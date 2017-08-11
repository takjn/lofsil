# LOFSIL
WIP

GR-PEACH、および、GR-LYCHEEで動作するサンプルプログラムです。  

## ビルド手順
開発環境の構築は、[GR-LYCHEE用オフライン開発環境の手順](https://developer.mbed.org/users/dkato/notebook/offline-development-lychee-langja/)を参照ください。

### GR-LYCHEEの場合
$ mbed compile -m GR_LYCHEE -t GCC_ARM --profile debug

### GR-PEACHの場合
$ mbed compile -m RZ_A1H -t GCC_ARM --profile debug

## 概要
USB0にUSBケーブルを接続してPC用アプリ [DisplayApp](https://developer.mbed.org/users/dkato/code/DisplayApp/)でカメラ映像が表示できます。

～参考ページ～  
[がじぇっとるねさすコミュニティ GR-LYCHEE OpenCV](http://japan.renesasrulz.com/gr_user_forum_japanese/f/gr-lychee/4208/gr-lychee-opencv)
