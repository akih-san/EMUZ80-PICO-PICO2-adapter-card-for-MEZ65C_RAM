# EMUZ80 PICO/PICO2 adapter card for MEZ65C_RAM

工事中（Under Construction）<br>

電脳伝説さんの[EMUZ80](https://vintagechips.wordpress.com/2022/03/05/emuz80_reference)は、レガシーCPUをmicrochip社のPIC18F47Q43/83/84マイコンで制御しています。<br>
このマイクロコントローラーを[Raspberry Pi PICO/PICO2](https://www.raspberrypi.com/products/raspberry-pi-pico/)に置き換えて、動かせないかと考えていました。<br>
PICO/PICO2のIO数（GPIO)は、PIC18F47Q43/83/84に比べてかなり少ないため、個々のレガシーCPU専用のアダプターにする必要があります。<br>
私のお気に入りのレガシーCPUは6502なので（Z80も好きです）
[MEZW65C_RAM](https://github.com/akih-san/MEZW65C_RAM-Rev2.1G)用のアダプターを試作してみました。<br>

試作経緯については、[Ｘ（旧ツイッター）](https://x.com/akih_san/status/2036215330545213747)にアップしていますので、興味のある方はご覧ください。<br>

・回路図<br>
・GPIO割り当て<br>
・部品表<br>


# 参考
FWを作成するときに、参考にしたサイトや書籍<br>

- [RP2350B EMU board](https://github.com/akih-san/RP2350B-EMU-BOARD-MEZW65C_RAM-Firmware)
- [MEZW65C_RAM-Rev2.1G](https://github.com/akih-san/MEZW65C_RAM-Rev2.1G)
- [MEZW65C_RAM-Rev2.0](https://github.com/akih-san/MEZW65C_RAM-Rev2.0)
- [MEZW65C_RAM-Rev2.1](https://github.com/akih-san/MEZW65C_RAM-Rev2.1)
- [Raspberry pi pico/pico2 The C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html)
- [Ｃ言語を使った高精度な組み込みシステム　ラズパイPicoベアメタル開発完全ガイド](https://www.amazon.co.jp/%EF%BC%A3%E8%A8%80%E8%AA%9E%E3%82%92%E4%BD%BF%E3%81%A3%E3%81%9F%E9%AB%98%E7%B2%BE%E5%BA%A6%E3%81%AA%E7%B5%84%E3%81%BF%E8%BE%BC%E3%81%BF%E3%82%B7%E3%82%B9%E3%83%86%E3%83%A0-%E3%83%A9%E3%82%BA%E3%83%91%E3%82%A4Pico%E3%83%99%E3%82%A2%E3%83%A1%E3%82%BF%E3%83%AB%E9%96%8B%E7%99%BA%E5%AE%8C%E5%85%A8%E3%82%AC%E3%82%A4%E3%83%89-%E7%B1%B3%E7%94%B0-%E8%81%A1-ebook/dp/B0FPDVJ626?ref_=ast_author_mpb)
- PIOつくし（準備号）「Norihiro Kumagai氏](https://github.com/tendai22)
- [ちょっとTea　Time!? PICO repeating_timerの憂鬱【備忘録】](http://www.easyaudiokit.com/bekkan2025/TeaTime2025/pico_repeating_timer/timernewpage2.html)
- [Raspberry Pi Pico C/C++ インターバルタイマ割り込みを試してみる](https://qiita.com/keyyum/items/3ce448c098c546dced20)
- [Raspberry Pi PicoでPWM出力](https://rikei-tawamure.com/entry/2021/02/08/213335)
- [5V系・3.3V系信号レベル変換](https://www.cepstrum.co.jp/hobby/5v33v/5v33v.html)
- [FatFs library for Raspberry Pi Pico / Pico 2](https://github.com/elehobica/pico_fatfs)


# 謝辞
このボードを作成するに至った経緯は、[DragonBallEZ氏](https://github.com/kyo-ta04/)の[Super AKI-80の組み立て方](https://note.com/quiet_duck4046/n/n32906e1dfb96?sub_rt=share_sb)と、[Pico2ROMEmu_PCB](https://github.com/kyo-ta04/Pico2ROMEmu_PCB)がきっかけで、Raspberry pi pico/pico2に興味が湧いたことからはじまります。<br>
[Norihiro Kumagai氏](https://github.com/tendai22)の[emuz80_pico2](https://github.com/tendai22/emuz80_pico2)は非常に参考になりました。<br>
### 参考にしたサイトと共に、とても感謝しています。

