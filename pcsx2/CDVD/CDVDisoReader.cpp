/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2010  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 *  Original code from libcdvd by Hiryu & Sjeep (C) 2002
 *  Modified by Florin for PCSX2 emu
 *  Fixed CdRead by linuzappz
 */

#include "PrecompiledHeader.h"
#include "IsoFileFormats.h"
#include "AsyncFileReader.h"
#include "common/FileSystem.h"
#include "CDVD/CDVD.h"
#include "common/Exceptions.h"

#include <cstring>
#include <array>

static InputIsoFile iso;

static int pmode, cdtype, currentTrackNum;

static s32 layer1start = -1;
static bool layer1searched = false;

static u32 maxLSN = 0;


static constexpr std::array<u32, 8> sizes = {{2352, 2048, 2352, 2336, 2048, 2324, 2332, 2352}};


void CALLBACK ISOclose()
{
	iso.Close();
}

inline u8 dec_to_bcd(u8 dec)
{
	return ((dec / 10) << 4) | (dec % 10);
}

inline void lsn_to_msf(u8* minute, u8* second, u8* frame, u32 lsn)
{
	*frame = dec_to_bcd(lsn % 75);
	lsn /= 75;
	*second = dec_to_bcd(lsn % 60);
	lsn /= 60;
	*minute = dec_to_bcd(lsn % 100);

	Console.WriteLn("MSF: ", " Frame: ", frame, " Second: ", second, " Minute: ", minute);
}

s32 CALLBACK ISOopen(const char* pTitle, int track)
{
	if ((pTitle == NULL) || (pTitle[0] == 0))
	{
		Console.Error("CDVDiso Error: No filename specified.");
		return -1;
	}

	if (track >= 0 && m_tracks.size() > 0)
	{
		currentTrackNum = cueFile->GetTrack(track)->trackNum;
		iso = *cueFile->GetTrack(currentTrackNum)->fileReader;
	}
	else
	{
		iso.Open(pTitle);
	}
	switch (iso.GetType())
	{
		case ISOTYPE_DVD:
			cdtype = CDVD_TYPE_PS2DVD;
			break;
		case ISOTYPE_AUDIO:
			cdtype = CDVD_TYPE_CDDA;
			break;
		default:
			cdtype = CDVD_TYPE_PS2CD;
			break;
	}

	layer1start = -1;
	layer1searched = false;

	return 0;
}

s32 CALLBACK ISOgetTN(cdvdTN* Buffer)
{
	Buffer->strack = 1;
	if (m_tracks.size() > 0)
		Buffer->etrack = m_tracks.size();
	else
		Buffer->etrack = 1;

	return 0;
}

static void CalculateDiskLength()
{
	u32 trackLength = 0;
	std::string oldFilePath = "";
	for(int i = 0; i < m_tracks.size(); i++)
	{
		oldFilePath = m_tracks[i].filePath;

		u64 fileSize = m_tracks[i].fileReader->GetBlockCount();

		//fileSize /= sizes[cueFile->GetTrack(i)->mode];
		if (!m_tracks[i].filePath.compare(oldFilePath.c_str()))
		{
			if (m_tracks[i].length.has_value())
			{
				trackLength = (m_tracks[i].length.value() - m_tracks[i].startLsn);
			}
			else
			{
				Console.Warning("File Size %d", fileSize);
				trackLength = static_cast<u32>(fileSize - m_tracks[i].startLsn);
			}
			maxLSN += trackLength;
		}
		else
		{
			trackLength += m_tracks[i].startLsn;
		}
	}
	Console.Warning("MaxLSN: %d", maxLSN);
}

s32 CALLBACK ISOgetTD(u8 Track, cdvdTD* Buffer)
{
	if (Track == 0)
	{
		if (Buffer == nullptr)
			return -1;
		try
		{
			Buffer->lsn = iso.GetBlockCount();
			Buffer->type = 0;
			return 0;
		}
		catch (...)
		{
			return -1;
		}
	}
	else
	{
		Buffer->type = cueFile->GetTrack(Track)->type;
		Buffer->lsn = cueFile->GetTrack(Track)->startLsn;
	}

	return 0;
}

static bool testForPrimaryVolumeDescriptor(const std::array<u8, CD_FRAMESIZE_RAW>& buffer)
{
	const std::array<u8, 6> identifier = {1, 'C', 'D', '0', '0', '1'};

	return std::equal(identifier.begin(), identifier.end(), buffer.begin() + iso.GetBlockOffset());
}

static void FindLayer1Start()
{
	if (layer1searched)
		return;

	layer1searched = true;

	std::array<u8, CD_FRAMESIZE_RAW> buffer;

	// The ISO9660 primary volume descriptor for layer 0 is located at sector 16
	iso.ReadSync(buffer.data(), 16);
	if (!testForPrimaryVolumeDescriptor(buffer))
	{
		Console.Error("isoFile: Invalid layer0 Primary Volume Descriptor");
		return;
	}

	// The volume space size (sector count) is located at bytes 80-87 - 80-83
	// is the little endian size, 84-87 is the big endian size.
	const int offset = iso.GetBlockOffset();
	uint blockresult = buffer[offset + 80] + (buffer[offset + 81] << 8) + (buffer[offset + 82] << 16) + (buffer[offset + 83] << 24);

	// If the ISO sector count is larger than the volume size, then we should
	// have a dual layer DVD. Layer 1 is on a different volume.
	if (blockresult < iso.GetBlockCount())
	{
		// The layer 1 start LSN contains the primary volume descriptor for layer 1.
		// The check might be a bit unnecessary though.
		if (iso.ReadSync(buffer.data(), blockresult) == -1)
			return;

		if (!testForPrimaryVolumeDescriptor(buffer))
		{
			Console.Error("isoFile: Invalid layer1 Primary Volume Descriptor");
			return;
		}
		layer1start = blockresult;
		Console.WriteLn(Color_Blue, "isoFile: second layer found at sector 0x%08x", layer1start);
	}
}

// Should return 0 if no error occurred, or -1 if layer detection FAILED.
s32 CALLBACK ISOgetDualInfo(s32* dualType, u32* _layer1start)
{
	FindLayer1Start();

	if (layer1start < 0)
	{
		*dualType = 0;
		*_layer1start = iso.GetBlockCount();
	}
	else
	{
		*dualType = 1;
		*_layer1start = layer1start;
	}
	return 0;
}

s32 CALLBACK ISOgetDiskType()
{
	return cdtype;
}

s32 CALLBACK ISOgetTOC(void* toc)
{
	u8 type = ISOgetDiskType();
	u8* tocBuff = (u8*)toc;

	//CDVD_LOG("CDVDgetTOC\n");

	if (type == CDVD_TYPE_DVDV || type == CDVD_TYPE_PS2DVD)
	{
		// get dvd structure format
		// scsi command 0x43
		memset(tocBuff, 0, 2048);

		FindLayer1Start();

		if (layer1start < 0)
		{
			// fake it
			tocBuff[0] = 0x04;
			tocBuff[1] = 0x02;
			tocBuff[2] = 0xF2;
			tocBuff[3] = 0x00;
			tocBuff[4] = 0x86;
			tocBuff[5] = 0x72;

			tocBuff[16] = 0x00;
			tocBuff[17] = 0x03;
			tocBuff[18] = 0x00;
			tocBuff[19] = 0x00;
			return 0;
		}
		else
		{
			// dual sided
			tocBuff[0] = 0x24;
			tocBuff[1] = 0x02;
			tocBuff[2] = 0xF2;
			tocBuff[3] = 0x00;
			tocBuff[4] = 0x41;
			tocBuff[5] = 0x95;

			tocBuff[14] = 0x60; // dual sided, ptp

			tocBuff[16] = 0x00;
			tocBuff[17] = 0x03;
			tocBuff[18] = 0x00;
			tocBuff[19] = 0x00;

			s32 l1s = layer1start + 0x30000 - 1;
			tocBuff[20] = (l1s >> 24);
			tocBuff[21] = (l1s >> 16) & 0xff;
			tocBuff[22] = (l1s >> 8) & 0xff;
			tocBuff[23] = (l1s >> 0) & 0xff;
		}
	}
	else if ((type == CDVD_TYPE_CDDA) || (type == CDVD_TYPE_PS2CDDA) ||
			 (type == CDVD_TYPE_PS2CD) || (type == CDVD_TYPE_PSCDDA) || (type == CDVD_TYPE_PSCD))
	{
		// cd toc
		// (could be replaced by 1 command that reads the full toc)
		u8 min, sec, frm;
		s32 i, err;
		cdvdTN diskInfo;
		cdvdTD trackInfo;
		memset(tocBuff, 0, 1024);
		if (ISOgetTN(&diskInfo) == -1)
		{
			diskInfo.etrack = 0;
			diskInfo.strack = 1;
		}
		if (ISOgetTD(1, &trackInfo) == -1)
			trackInfo.lsn = 0;

		tocBuff[0] = 0x41;
		tocBuff[1] = 0x00;

		//Number of FirstTrack
		tocBuff[2] = 0xA0;
		tocBuff[7] = itob(diskInfo.strack);

		//Number of LastTrack
		tocBuff[12] = 0xA1;
		tocBuff[17] = itob(diskInfo.etrack);

		if (m_tracks.size() <= 0)
		{
			lsn_to_msf(trackInfo.lsn, &min, &sec, &frm);
			tocBuff[22] = 0xA2;
			tocBuff[27] = itob(min);
			tocBuff[28] = itob(sec);
		}
		// There were multiple tracks
		if (m_tracks.size() > 0)
		{
			CalculateDiskLength();
			lsn_to_msf(maxLSN, &min, &sec, &frm);
			Console.Warning("MAX LSN: %d", maxLSN);
			tocBuff[22] = 0xA2;
			tocBuff[27] = itob(min);
			tocBuff[28] = itob(sec);
		}
		for (i = diskInfo.strack; i <= diskInfo.etrack; i++)
		{
			err = ISOgetTD(i, &trackInfo);
			lsn_to_msf(trackInfo.lsn, &min, &sec, &frm);
			tocBuff[i * 10 + 30] = trackInfo.type;
			tocBuff[i * 10 + 32] = err == -1 ? 0 : itob(i); //number
			tocBuff[i * 10 + 37] = itob(min);
			tocBuff[i * 10 + 38] = itob(sec);
			tocBuff[i * 10 + 39] = itob(frm);
		}
	}
	else
		return -1;

for(int i=0; i<2048/10; i+=10)
{
	Console.Warning("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
	tocBuff[i+0],
	tocBuff[i+1],
	tocBuff[i+2],
	tocBuff[i+3],
	tocBuff[i+4],
	tocBuff[i+5],
	tocBuff[i+6],
	tocBuff[i+7],
	tocBuff[i+8],
	tocBuff[i+9]);
}
	return 0;
}

s32 CALLBACK ISOreadSector(u8* tempbuffer, u32 lsn, int mode)
{
	static u8 cdbuffer[CD_FRAMESIZE_RAW] = {0};

	int _lsn = lsn;

	if (_lsn <= 0)
		lsn = iso.GetBlockCount() + _lsn;
	
	if (lsn >= iso.GetBlockCount())
		return -1;

	if (mode == CDVD_MODE_2352)
	{
		iso.ReadSync(tempbuffer, lsn);
		return 0;
	}

	iso.ReadSync(cdbuffer, lsn);


	u8* pbuffer = cdbuffer;
	int psize = 0;

	switch (mode)
	{
			//case CDVD_MODE_2352:
			// Unreachable due to shortcut above.
			//	pxAssume(false);
			//	break;

		case CDVD_MODE_2340:
			pbuffer += 12;
			psize = 2340;
			break;
		case CDVD_MODE_2328:
			pbuffer += 24;
			psize = 2328;
			break;
		case CDVD_MODE_2048:
			pbuffer += 24;
			psize = 2048;
			break;

			jNO_DEFAULT
	}

	memcpy(tempbuffer, pbuffer, psize);

	return 0;
}

s32 CALLBACK ISOreadTrack(u32 lsn, int mode, int track = 0)
{
	int _lsn = lsn;

	if (track > 0 && track != currentTrackNum)
	{
		currentTrackNum = cueFile->GetTrack(track)->trackNum;
		iso = *cueFile->GetTrack(currentTrackNum)->fileReader;
 	}

	if (_lsn < 0)
		lsn = iso.GetBlockCount() + _lsn;

	iso.BeginRead2(lsn);

	pmode = mode;

	return 0;
}

s32 CALLBACK ISOreadSubQ(u32 lsn, cdvdSubQ* subq)
{
	// fake it - NO!
	u8 min, sec, frm;

	// TODO: PS1 Libcrypt games will look like garbage data. This is a key being generated in SubQ that needs to be passed on!

	s32 err = 0;

	memset(subq, 0, sizeof(cdvdSubQ));

	lsn_to_msf(lsn, &min, &sec, &frm);
	subq->trackM = itob(min);
	subq->trackS = itob(sec);
	subq->trackF = itob(frm);

	u8 i = 1;
	while (i < m_tracks.size() && lsn >= cueFile->GetTrack(i + 1)->startLsn)
		++i;

	lsn -= cueFile->GetTrack(i)->startLsn;

	lsn_to_msf(lsn + (2 * 75), &min, &sec, &frm);
	subq->discM = itob(min);
	subq->discS = itob(sec);
	subq->discF = itob(frm);

	Console.Warning("LSN: %d", lsn);

	subq->mode = cueFile->GetTrack(i)->mode;
	subq->ctrl = cueFile->GetTrack(i)->type;
	// This is for error checking sake. Tracks on disk start at one!
	subq->trackNum = cueFile->GetTrack(i)->trackNum;
	subq->trackIndex = 1;

	Console.Warning("Track num: %d", subq->trackNum);

	return err;
}

s32 CALLBACK ISOgetBuffer(u8* buffer)
{
	return iso.FinishRead3(buffer, pmode);
}

//u8* CALLBACK ISOgetBuffer()
//{
//	iso.FinishRead();
//	return pbuffer;
//}

s32 CALLBACK ISOgetTrayStatus()
{
	return CDVD_TRAY_CLOSE;
}

s32 CALLBACK ISOctrlTrayOpen()
{
	return 0;
}
s32 CALLBACK ISOctrlTrayClose()
{
	return 0;
}

s32 CALLBACK ISOdummyS32()
{
	return 0;
}

void CALLBACK ISOnewDiskCB(void (*/* callback */)())
{
}

CDVD_API CDVDapi_Iso =
	{
		ISOclose,

		ISOopen,
		ISOreadTrack,
		ISOgetBuffer,
		ISOreadSubQ,
		ISOgetTN,
		ISOgetTD,
		ISOgetTOC,
		ISOgetDiskType,
		ISOdummyS32, // trayStatus
		ISOdummyS32, // trayOpen
		ISOdummyS32, // trayClose

		ISOnewDiskCB,

		ISOreadSector,
		ISOgetDualInfo,
};
