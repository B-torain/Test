��    H      \  a   �         �   !  9   �  Y   �  R   ?  b   �  b   �  w   X  o   �  �   @	  �   
  ~   �
  �   *  `   �  Q   R  �   �  �   :  /     M   >  �   �  ]   M  �   �  G   W  H   �  d   �  �   M  b   �  �   B  �     5   �  6   �  �     �   �  �   �  �   E  �     r  �  |   0  �   �  �  �  N  )  �   x  �   x  �   U  �   �  �   �   z  u!  J   �"  h   ;#  �   �#  p   9$  G  �$  �   �%  �   v&  �   '  �   �'  �   u(  �   )  �   �)  �   �*  �   u+  �   ,  *  �,  �   �-  �   �.  \   �/  r   0  �   �0  �   1  �   2  �   �2  �  _3  [  �4  �   F7  I   �7  L   58  `   �8  _   �8  |   C9  g   �9  {   (:  �   �:  �   �;  �   Q<  "  �<  c   >  }   l>  �   �>    �?  *   �@  R   �@  �   A  U   �A  �   LB  Y   C  B   nC  Z   �C  �   D  �   �D  �   DE  �   +F  7   �F  7   G  �   KG  ~   2H  �   �H  �   �I  �   qJ  �  (K  �   �L    LM  t  TN  �  �O  S  bQ  �   �R  �   yS  �   MT    U  �  )V  L   �W  a   �W  {   aX  y   �X  �  WY  �   �Z  �   [  �   "\  �   �\  �   �]  0  _^  �   �_  �   ``  �   2a  �   �a    �b  2  �c    �d  d   f  �   mf  �   �f  �   �g  �   gh    Si  �  Vj                          	   ;   B   -         6      <   *   1               2   A   7   5                    '       #   .      F       G   C   $   !      :   )   ,       4   =   (   D       E   @       &      "             3         9       >       0   /                                     8                    H              ?   
   %             +        'Ignore' to continue waiting for the thread to respond.
'Cancel' to attempt to cancel the thread.
'Terminate' to quit PCSX2 immediately.
 0 - Disables VU Cycle Stealing.  Most compatible setting! 1 - Default cyclerate. This closely matches the actual speed of a real PS2 EmotionEngine. 1 - Mild VU Cycle Stealing.  Lower compatibility, but some speedup for most games. 2 - Moderate VU Cycle Stealing.  Even lower compatibility, but significant speedups in some games. 2 - Reduces the EE's cyclerate by about 33%.  Mild speedup for most games with high compatibility. 3 - Maximum VU Cycle Stealing.  Usefulness is limited, as this will cause flickering visuals or slowdown in most games. 3 - Reduces the EE's cyclerate by about 50%.  Moderate speedup, but *will* cause stuttering audio on many FMVs. All plugins must have valid selections for %s to run.  If you are unable to make a valid selection due to missing plugins or an incomplete install of %s, then press Cancel to close the Configuration panel. Avoids memory card corruption by forcing games to re-index card contents after loading from savestates.  May not be compatible with all games (Guitar Hero). Check HDLoader compatibility lists for known games that have issues with this. (Often marked as needing 'mode 1' or 'slow DVD' Check this to force the mouse cursor invisible inside the GS window; useful if using the mouse as a primary control device for gaming.  By default the mouse auto-hides after 2 seconds of inactivity. Completely closes the often large and bulky GS window when pressing ESC or pausing the emulator. Enable this if you think MTGS thread sync is causing crashes or graphical errors. Enables automatic mode switch to fullscreen when starting or resuming emulation. You can still toggle fullscreen display at any time using alt-enter. Existing %s settings have been found in the configured settings folder.  Would you like to import these settings or overwrite them with %s default values?

(or press Cancel to select a different settings folder) Failed: Destination memory card '%s' is in use. Failed: Duplicate is only allowed to an empty PS2-Port or to the file system. Gamefixes can work around wrong emulation in some titles. 
They may also cause compatibility or performance issues.

The safest way is to make sure that all game fixes are completely disabled. Known to affect following games:
 * Bleach Blade Battler
 * Growlanser II and III
 * Wizardry Known to affect following games:
 * Digital Devil Saga (Fixes FMV and crashes)
 * SSX (Fixes bad graphics and crashes)
 * Resident Evil: Dead Aim (Causes garbled textures) Known to affect following games:
 * Mana Khemia 1 (Going "off campus")
 Known to affect following games:
 * Test Drive Unlimited
 * Transformers NTFS compression can be changed manually at any time by using file properties from Windows Explorer. NTFS compression is built-in, fast, and completely reliable; and typically compresses memory cards very well (this option is highly recommended). Note that when Framelimiting is disabled, Turbo and SlowMotion modes will not be available either. Note: Recompilers are not necessary for PCSX2 to run, however they typically improve emulation speed substantially. You may have to manually re-enable the recompilers listed above, if you resolve the errors. Notice: Due to PS2 hardware design, precise frame skipping is impossible. Enabling it will cause severe graphical errors in some games. Notice: Most games are fine with the default options. Notice: Most games are fine with the default options.  Out of Memory (sorta): The SuperVU recompiler was unable to reserve the specific memory ranges required, and will not be available for use.  This is not a critical error, since the sVU rec is obsolete, and you should use microVU instead anyway. :) PCSX2 is unable to allocate memory needed for the PS2 virtual machine. Close out some memory hogging background tasks and try again. PCSX2 requires a *legal* copy of the PS2 BIOS in order to run games.
You cannot use a copy obtained from a friend or the Internet.
You must dump the BIOS from your *own* Playstation 2 console. PCSX2 requires a PS2 BIOS in order to run.  For legal reasons, you *must* obtain a BIOS from an actual PS2 unit that you own (borrowing doesn't count).  Please consult the FAQs and Guides for further instructions. Playstation game discs are not supported by PCSX2.  If you want to emulate PSX games then you'll have to download a PSX-specific emulator, such as ePSXe or PCSX. Please ensure that these folders are created and that your user account is granted write permissions to them -- or re-run PCSX2 with elevated (administrator) rights, which should grant PCSX2 the ability to create the necessary folders itself.  If you do not have elevated rights on this computer, then you will need to switch to User Documents mode (click button below). Please select a valid BIOS.  If you are unable to make a valid selection then press Cancel to close the Configuration panel. Please select your preferred default location for PCSX2 user-level documents below (includes memory cards, screenshots, settings, and savestates).  These folder locations can be overridden at any time using the Plugin/BIOS Selector panel. Primarily targetting the EE idle loop at address 0x81FC0 in the kernel, this hack attempts to detect loops whose bodies are guaranteed to result in the same machine state for every iteration until a scheduled event triggers emulation of another unit.  After a single iteration of such loops, we advance to the time of the next event or the end of the processor's timeslice, whichever comes first. Removes any benchmark noise caused by the MTGS thread or GPU overhead.  This option is best used in conjunction with savestates: save a state at an ideal scene, enable this option, and re-load the savestate.

Warning: This option can be enabled on-the-fly but typically cannot be disabled on-the-fly (video will typically be garbage). Runs VU1 on its own thread (microVU1-only). Generally a speedup on CPUs with 3 or more cores. This is safe for most games, but a few games are incompatible and may hang. In the case of GS limited games, it may be a slowdown (especially on dual core CPUs). Setting higher values on this slider effectively reduces the clock speed of the EmotionEngine's R5900 core cpu, and typically brings big speedups to games that fail to utilize the full potential of the real PS2 hardware. Speedhacks usually improve emulation speed, but can cause glitches, broken audio, and false FPS readings.  When having emulation problems, disable this panel first. The PS2-slot %d has been automatically disabled.  You can correct the problem
and re-enable it at any time using Config:Memory cards from the main menu. The Presets apply speed hacks, some recompiler options and some game fixes known to boost speed.
Known important game fixes will be applied automatically.

--> Uncheck to modify settings manually (with current preset as base) The Presets apply speed hacks, some recompiler options and some game fixes known to boost speed.
Known important game fixes will be applied automatically.

Presets info:
1 -     The most accurate emulation but also the slowest.
3 --> Tries to balance speed with compatibility.
4 -     Some more aggressive hacks.
6 -     Too many hacks which will probably slow down most games.
 The specified path/directory does not exist.  Would you like to create it? The thread '%s' is not responding.  It could be deadlocked, or it might just be running *really* slowly. There is not enough virtual memory available, or necessary virtual memory mappings have already been reserved by other processes, services, or DLLs. This action will reset the existing PS2 virtual machine state; all current progress will be lost.  Are you sure? This command clears %s settings and allows you to re-run the First-Time Wizard.  You will need to manually restart %s after this operation.

WARNING!!  Click OK to delete *ALL* settings for %s and force-close the app, losing any current emulation progress.  Are you absolutely sure?

(note: settings for plugins are unaffected) This folder is where PCSX2 records savestates; which are recorded either by using menus/toolbars, or by pressing F1/F3 (save/load). This folder is where PCSX2 saves its logfiles and diagnostic dumps.  Most plugins will also adhere to this folder, however some older plugins may ignore it. This folder is where PCSX2 saves screenshots.  Actual screenshot image format and style may vary depending on the GS plugin being used. This hack works best for games that use the INTC Status register to wait for vsyncs, which includes primarily non-3D RPG titles. Games that do not use this method of vsync will see little or no speedup from this hack. This is the folder where PCSX2 saves your settings, including settings generated by most plugins (some older plugins may not respect this value). This recompiler was unable to reserve contiguous memory required for internal caches.  This error can be caused by low virtual memory resources, such as a small or disabled swapfile, or by another program that is hogging a lot of memory. This slider controls the amount of cycles the VU unit steals from the EmotionEngine.  Higher values increase the number of cycles stolen from the EE for each VU microprogram the game runs. This wizard will help guide you through configuring plugins, memory cards, and BIOS.  It is recommended if this is your first time installing %s that you view the readme and configuration guide. Updates Status Flags only on blocks which will read them, instead of all the time. This is safe most of the time, and Super VU does something similar by default. Vsync eliminates screen tearing but typically has a big performance hit. It usually only applies to fullscreen mode, and may not work with all GS plugins. Warning!  Changing plugins requires a complete shutdown and reset of the PS2 virtual machine. PCSX2 will attempt to save and restore the state, but if the newly selected plugins are incompatible the recovery may fail, and current progress will be lost.

Are you sure you want to apply settings now? Warning!  You are running PCSX2 with command line options that override your configured plugin and/or folder settings.  These command line options will not be reflected in the settings dialog, and will be disabled when you apply settings changes here. Warning!  You are running PCSX2 with command line options that override your configured settings.  These command line options will not be reflected in the Settings dialog, and will be disabled if you apply any changes here. Warning: Some of the configured PS2 recompilers failed to initialize and have been disabled: When checked this folder will automatically reflect the default associated with PCSX2's current usermode setting.  You are about to delete the formatted memory card '%s'. All data on this card will be lost!  Are you absolutely and quite positively sure? You can change the preferred default location for PCSX2 user-level documents here (includes memory cards, screenshots, settings, and savestates).  This option only affects Standard Paths which are set to use the installation default value. You may optionally specify a location for your PCSX2 settings here.  If the location contains existing PCSX2 settings, you will be given the option to import or overwrite them. Your system is too low on virtual resources for PCSX2 to run. This can be caused by having a small or disabled swapfile, or by other programs that are hogging resources. Zoom = 100: Fit the entire image to the window without any cropping.
Above/Below 100: Zoom In/Out
0: Automatic-Zoom-In untill the black-bars are gone (Aspect ratio is kept, some of the image goes out of screen).
  NOTE: Some games draw their own black-bars, which will not be removed with '0'.

Keyboard: CTRL + NUMPAD-PLUS: Zoom-In, CTRL + NUMPAD-MINUS: Zoom-Out, CTRL + NUMPAD-*: Toggle 100/0 Project-Id-Version: PCSX2 1.0.0
Report-Msgid-Bugs-To: https://github.com/PCSX2/pcsx2/issues
POT-Creation-Date: 2015-10-23 19:59+0200
PO-Revision-Date: 2015-04-22 18:25+0200
Last-Translator: Ceyhun Özgöç <atiamar@hotmail.com>
Language-Team: Ceyhun Özgöç (PyramidHead) <atiamar@hotmail.com>
Language: tr_TR
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Poedit-KeywordsList: pxE;pxEt
X-Poedit-SourceCharset: UTF-8
X-Poedit-Basepath: C:\Users\Ceyhun\Downloads\PCSX2 SVN
X-Generator: Poedit 1.6.9
X-Poedit-SearchPath-0: pcsx2
X-Poedit-SearchPath-1: common
 İşlemin yanıt vermesini beklemek için 'Yoksay'a tıklayın.
İşlemi iptal etmek için 'İptal'e tıklayın.
PCSX2'den çıkmak için 'Sonlandır'a tıklayın.
 0 - VU Cycle Stealing'i devre dışı bırakır. En sorunsuz seçenektir! 1 - Varsayılan döngü oranı. Gerçek PS2 EE hızına oldukça yakındır. 1 - Mild VU Cycle Stealing. Uyumluluğu düşüktür ancak çoğu oyunda hız artışı sağlar. 2 - Moderate VU Cycle Stealing. Daha düşük uyumluluk; birçok oyunda büyük hız artışı. 2 - EE'nin döngü oranını %33 oranında azaltır. Birçok oyunda hız artışı sağlar, uyumluluğu oldukça yüksektir. 3 - Maximum VU Cycle Stealing. Titremeye ve takılmalara neden olacağından faydaları sınırlıdır. 3 - EE'nin döngü oranını %50 oranında azaltır. Ortalama hız artışı sağlamasına rağmen takılmalara neden olur. %s'nin çalışması için tüm eklentilerin seçilmiş olması gerekir. %s yüklemesinde oluşan bir hata sonucu veya eksik eklentiler nedeniyle seçim yapamıyorsanız İptal'i tıklayarak Yapılandırma panelini kapatın. Kayıt konumlarının yüklenmesinden sonra oyunları kart içeriklerini yeniden dizinlemeleri konusunda zorlayarak hafıza kartı bozulmalarını önler. Tüm oyunlarla uyumlu olmayabilir. (Guitar Hero) Bilinen sorunlu oyunlar listesini görmek için HDLoader uyumluluk listesine bakın. (Sorunlu oyunlar 'mode1' ya da 'slow DVD' olarak işaretlidir) Fare imlecinin GS ekranı üzerinde görünmesini istemiyorsanız bunu etkinleştirin. Özellikle fareyi birincil kontrol cihazı olarak kullanıyorsanız oldukça faydalıdır. Varsayılan olarak 2 saniye boyunca herhangi bir işlem yapılmadığında fare imleci otomatik olarak gizlenir. ESC tuşuna basıldığında ya da emülatör duraklatıldığında GS ekranı tamamen kapatılır. MTGS işlem eşzamanlamasının çökmelere veya görsel bozulmalara neden olduğunu düşünüyorsanız bunu etkinleştirin. Emülatör çalıştığında ya da devam ettirildiğinde otomatik olarak tam ekrana geçişi etkinleştirir. Tam ekrandan çıkmak için Alt+Enter'ı kullanabilirsiniz. Yapılandırılmış ayarlar klasöründe mevcut %s ayarları bulundu. Bu ayarları içe aktarıp uygulamak mı yoksa bunları varsayılan %s değerleri ile değiştirmek mi istiyorsunuz?

(veya farklı bir ayarlar klasörü seçmek için İptal'i tıklayın) Hata: Hedef %s hafıza kartı kullanımda. Hata: Kopyalama yalnızca boş bir PS2-Portu veya dosya sistemi için geçerlidir. Oyun düzeltmeleri bazı oyunlarda işe yaramayabilir. 
Bunun yanı sıra uyumluluk ya da performans sorunlarına neden olabilirler.

En güvenli seçenek tüm oyun düzeltmelerinin tamamen devre dışı bırakılmasıdır. Etkilediği bilinen oyunlar:
* Bleach Blade Battler
* Growlanser II ve III
* Wizardry Etkilediği bilinen oyunlar:
* Digital Devil Saga (Videoları ve çökmeleri düzeltir)
* SSX (Görüntüleri ve çökmeleri düzeltir)
* Resident Evil: Dead Aim (Görüntüde bozulmalara neden olur) Etkilediği bilinen oyunlar:
* Mana Khemia 1 (Haritadan dışarı çıkma hatası için)
 Etkilediği bilinen oyunlar:
* Test Drive Unlimited
* Transformers NTFS sıkıştırması Windows Gezgini dosya seçeneklerininden el ile değiştirilebilir. NTFS sıkıştırma seçeneği yerleşiktir, hızlıdır ve tamamen güvenilirdir. Hafıza kartının diskte kapladığı boyutu oldukça azaltır. (bu seçenek önerilir.) Kare sınırlaması devre dışı bırakıldığında Turbo ve Ağır Çekim modlarının da devre dışı bırakılacağını unutmayın. Not:  Derleyiciler PCSX2'nin çalışması için gerekli değildir; ancak emülatör hızını oldukça artırırlar. Sorun ortadan kalktıktan sonra yukarıda listelenen derleyicileri el ile yeniden etkinleştirmeniz gerekebilir. Not: PS2 donanım tasarımı nedeniyle kusursuz kare atlama özelliği imkansızdır. Bu özelliği etkinleştirmek bazı oyunlarda ciddi görsel bozulmalara neden olacaktır. Not: Birçok oyun varsayılan ayarlarla çalışabilir. Not: Birçok oyun varsayılan ayarlarla çalışabilir. Hafıza hatası: SuperVU derleyicisi belirtilen hafıza değerlerini ayıramadığı için kullanılamıyor. Bu hata sVU zaten eski olduğundan ve onun yerine microVU kullansanız daha iyi olacağından çok da ciddi değildir. :) PCSX2, PS2 sanal makinesi için gerekli hafızayı ayıramıyor. Arkaplanda çalışan uygulamaları kapatıp yeniden deneyin. PCSX2'nin çalışması için *yasal* bir PS2 BIOS dosyası gerekir.
İnternetten veya bir arkadaşınızdan aldığınız bir kopya kullanamazsınız.
BIOS dosyanızı *kendi* Play Station 2 konsolunuzdan almalısınız. PCSX2'nin çalışması için bir PS2 BIOS dosyası gereklidir. Yasal nedenlerden dolayı BIOS dosyasını *kendi* PS2 konsolunuzdan almalısınız. Daha fazla talimat için lütfen SSS ve kullanma kılavuzlarına danışın. PCSX2 Play Station disklerini desteklemez. Bir PS oyunu oynamak istiyorsanız bunun için ePSXe ya da PCSX gibi PS oyunlarına yönelik yapılmış bir emülatör kullanmalısınız. Lütfen bu klasörlerin oluşturulduğundan ve kullanıcı hesabınızına dosya yazma haklarının verildiğinden emin olun veya PCSX2'ye gerekli klasörleri oluşturma yetkisi vermek için PCSX2'yi yükseltilmiş (yönetici) haklar ile tekrar çalıştırın. Bu bilgisayarda yükseltilmiş haklara sahip değilseniz aşağıdan Kullanıcı Dosyaları moduna geçmeniz gerekir (aşağıdaki düğmeyi tıklayın). Lütfen geçerli bir BIOS seçiniz. Geçerli bir seçim yapamıyorsanız Yapılandırma panelini kapatmak için İptal'e basınız. Lütfen PCSX2 hafıza kartları, ekran görüntüleri, ayarlar ve kayıt konumları gibi PCSX2 kullanıcı dosyalarının saklanacağı varsayılan konumu seçiniz. Bu klasörlerin konumları daha sonradan Eklenti/BIOS Seçici paneli altından değiştirilebilir. Kernelin 0x81FC0 adresindeki EE işlevsiz döngüsünü hedef alan bu hack farklı bir olay planlanmış işlem ünitesini değiştirene dek her bir iterasyon sonucu gövdeleri aynı makine durumunda oluşacak olan döngüleri tespit eder. Bu döngülerin tek seferlik iterasyonundan sonra işlemcinin zaman döngüsüne ya da bir sonraki olayın zamanlamasına ilerlenir. MTGS işlemi ya da ekran kartının aşırı derece ısınması nedeniyle oluşabilecek kalite testi gürültüsünü kaldırır. Bu seçenek kayıt konumlarıyla bağlantılı olarak kullanılır: herhangi bir konumda oyununuzu kaydedin, bu seçeneği etkinleştirin ve kayıt konumunuzu yeniden yükleyin.

Dikkat: Bu seçenek oyun açıkken etkinleştirilebilir fakat kapatılamaz (görüntüler bozulur). VU1'i kendi işlemi altında yürütür (yalnızca microVU1 için geçerlidir). 3 ya da daha fazla çekirdeği olan işlemcilerde hız artışı sağlar. Birçok oyun için güvenlidir fakat birkaç oyunla uyumsuz olduğundan donmalar meydana gelebilir. GS sınırlı oyunlarda FPS düşebilir (özellikle çift çekirdekli işlemcilerde). Buradan yüksek değerleri seçtiğinizde EE'nin R5900 çekirdek işlemcisinin saat hızı azaltılarak gerçek PS2 donanımı seviyesine ulaşamayan oyunlarda büyük hız artışı sağlanır. Hız Hackleri emülatör genelde emülatör hızını artırır; ancak seste bozulmalara ve yanlış FPS okumalarına neden olabilir. Oyunlarda sorunlar yaşarsanız ilk olarak bu paneli devre dışı bırakın. %d PS2-slotu otomatik olarak devre dışı bırakıldı. Sorunu dilediğiniz zaman ana menüden
Yapılandırma:Hafıza kartları menüsünü kullanarak düzeltebilir ve bunu yeniden etkinleştirebilirsiniz. Ön Ayarlar hızı artırdığı bilinen bazı derleyici seçenekleri, oyun düzeltmeleri ve hız hackleri uygular.
Bilinen önemli oyun düzeltmeleri otomatik olarak uygulanır.

--> Ayarları (seçili ön ayarı temel alarak) el ile yapmak için işareti kaldırın. Ön ayarlar hızı artırdığı bilinen bazı derleyici seçenekleri, oyun düzeltmeleri ve hız hacklerini uygular.
Bilinen önemli oyun düzeltmeleri otomatik olarak uygulanır.

Ön ayarlar hakkında:
1 --> En uyumlu fakat en yavaş.
3 --> Hızla uyumluluğu dengeler.
4 --> Bazı agresif hackler uygulanır.
6 --> Muhtemelen oyunu yavaşlatmaya neden olacak fazlalıkta hack uygulanır.
 Seçili konum/klasör bulunamadı. Bu klasörü oluşturmak istiyor musunuz? %s işlemi yanıt vermiyor. Kilitlenmiş ya da *aşırı derecede* yavaş çalışıyor olabilir. Yetersiz sanal bellek miktarı veya gerekli sanal bellek diğer işlemler, hizmetler ya da DLL'ler tarafından ayrılmış. PS2 sanal makinesi baştan başlatılacak; şu anki konumunuzu kaybedeceksiniz. Bunu yapmak istediğinizden emin misiniz? Bu komut tüm %s ayarlarını silerek İlk Kullanım Sihirbazını yeniden yürütmenizi sağlar. Bunu seçtikten sonra %s'i el ile yeniden başlatmalısınız.

DİKKAT!! *TÜM* %s ayarlarını silmek ve şu anki konumunuzu kaybederek uygulamayı tamamen sonlandırmak için Tamam'a tıklayın. Bunu yapmak istediğinizden kesinlikle emin misiniz?

(not: eklentilerin ayarları etkilenmez) Bu klasör F1/F3 (kaydet/yükle)'e basarak veya menü/araç çubuklarını kullanarak kaydedebileceğiniz PCSX2 kayıt konumlarının saklandığı klasördür. Burası PCSX2'nin günlük dosyalarını ve tanımlama dökümlerini kaydettiği klasördür. Bazı eski eklentiler hariç birçok eklenti bu klasörü kullanır. Bu klasör PCSX2'nin ekran görüntülerini kaydettiği klasördür. Ekran görüntüsünün dosya biçimi ve stili kullanılan GS eklentisine göre değişebilir. Bu hack en çok özellikle 3D olmayan ve vsync'i beklemek için INTC status'u kullanan oyunlarla uyumludur. Bu vsync yöntemini kullanmayan oyunlarda hiç hız artışı görülmez veya çok az bir hız artışı görülür. Bu klasör PCSX2'nin birçok eklenti tarafından oluşturulan ayarlar da dahil olmak üzere (bazı daha eski eklentiler bu kurala uymayabilir) ayarlarınızı kaydettiği klasördür. Derleyici dahili önbellek için gerekli olan bitişik hafıza miktarını ayıramıyor. Bu hata takas dosyasının küçük olması ya da devre dışı bırakılması nedeniyle düşük sanal bellek miktarı olan bilgisayarlarda veya başka bir programın hafızanın tamamı kullanması nedeniyle olur. Bu kaydırıcı, VU ünitesinin EE'den ne kadar döngü eksilttiğini ayarlamanızı sağlar. Daha yüksek değerler oyunun yürüttüğü her bir VU mikro proframı için çalınan EE sayısını artırır. Bu sihirbaz size eklentileri, hafıza kartlarını ve BIOS'u ayarlamanız konusunda yardımcı olacaktır. %s'i ilk kez yüklediyseniz beni oku dosyasına ve yapılandırma kılavuzuna göz atmanız önerilir. Status Flag'lerini her zaman yerine yalnızca okunacakları zaman günceller. Bu çoğu zaman güvenlidir ve zaten Super VU varsayılan olarak buna benzer bir işlem uygular. Vsync ekran üzerinde oluşan bölünmeleri önlemesine karşın performans kaybına neden olur. Genel olarak yalnızca tam ekran modunda çalışır ve tüm GS eklentileri ile uyumlu olmayabilir. Dikkat! Eklentileri değiştirdikten sonra PS2 sanal makinesini baştan başlatmanı gerekir. PCSX2 şu anki konumunuzu kaydetmeyi deneyecek fakat yeni seçilen eklentiler uyumlu değilse şu anki konumunuzu kaybedeceksiniz.

Ayarları uygulamak istediğinize emin misiniz? Dikkat! PCSX2'yi yapılandırılmış eklenti ve/veya klasör ayarlarınızı etkileyecek komut satırı seçenekleriyle çalıştırıyorsunuz. Bu komut satırı seçenekleri doğrudan Ayarlar sekmesine yansıtılmaz ve buradan yapılan herhangi bir değişiklik komut satırını devre dışı bırakır. Dikkat! PCSX2'yi yapılandırılmış ayarlarınızı etkileyecek komut satırı seçenekleriyle çalıştırıyorsunuz. Bu komut satırı seçenekleri doğrudan Ayarlar penceresine yansıtılmaz ve buradan yapılan herhangi bir değişiklik komut satırını devre dışı bırakır. Uyarı: Yapılandırılmış bazı PS2 derleyicileri başlatılamadı ve devre dışı bırakıldı. Seçildiğinde bu klasör, PCSX2'nin şu anki ilişkilendirilmiş varsayılan kullanıcı modu ayarını otomatik olarak yansıtır. Biçimlendirilmiş '%s' hafıza kartını silmek üzeresiniz. Karttaki tüm veriler kaybolacaktır! Bunu yapmak istediğinizden kesinlikle emin misiniz? PCSX2 kullanıcı düzeyi dosyaların saklandığı tercih edilen varsayılan konumu buradan değiştirebilirsiniz. Bu seçenek yalnızca yükleme sırasında varsayılan olarak ayarlanmış Standart Konumları etkiler. Buradan isteğe bağlı olarak PCSX2 ayarlarınız için bir konum belirleyebilirsiniz. Seçtiğiniz klasör mevcut PCSX2 ayarları içeriyorsa, bunları 'İçe Aktar' veya 'Üzerine Yaz' seçeneklerinden birini seçmeniz istenecektir. Sistem PCSX2'nin çalışması için yeterli sanal kaynağa sahip değil. Bu hata takas dosyasının çok küçük boyutlu olması ya da devre dışı bırakılması veya arkaplanda çalışan uygulamaların hafızanın tamamını kullanması sonucu oluşur. Yakınlaştırma = 100: Görüntüyü kesmeden ekrana sığdır.
100 üzeri veya altı: Yakınlaştır/Uzaklaştır
0: Siyah çizgiler yok olana kadar otomatik yakınlaştırma yap (en-boy oranı bozulmaz, bazı görüntüler ekran dışına taşabilir).
NOT: Bazı oyunlar kendi siyah çizgilerini çizdiklerinden '0' bu çizgileri silemez.

Klavye: CTRL + NUMPAD-ARTI: Yakınlaştırma, CTRL + NUMPAD - EKSİ: Uzaklaştırma, CTRL + NUMPAD - *: 100/0 