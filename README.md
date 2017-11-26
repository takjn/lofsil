# LOFSIL - ヒートマップによる実空間行動分析

GR デザインコンテスト 2017 エントリー作品です。GR-PEACHと2台のビデオカメラで生成した３次元空間データを元に、ヒートマップを生成するプロトタイプを作成しました。

ヒートマップは店舗やオフィス、工場内と言った実空間における⾏動分析の可視化手法として用いられており、導線分析や滞留分析、またそれに基づく店舗改善、業務改善などに用いられています。

[![デモ動画](https://img.youtube.com/vi/YyHcLxZKYf0/0.jpg)](https://www.youtube.com/watch?v=YyHcLxZKYf0)

赤いところが動きが無いところ、黄色いところが動きがあったところを表します。人が手をあげたり下げたり歩いたり立ち止まっていることが見て取れることと思います。 

このプロトタイプは人がいる位置に応じたコンテンツを表示するデジタルサイネージを意識したものとなっています。周囲に6枚のパネル(がじぇるねボードの写真)があり、人がその正面に立つとそのパネルが拡大表示されます。床に見えないスイッチがあるようなものであり、実際には音声などで目の前にある商品の解説などを行うことを想定しています。 

プロトタイプの応用として監視カメラや見守りカメラなどへの活用も考えられます。動きが無い箇所が赤く表示されるため、不審者の滞留検知や通報システムなどへの応用が考えられます。 


## システム概要

★TBD

3Dヒートマップの元データとなる3次元空間データは2台のカメラ画像を元にGR-PEACH上でリアルタイムに生成します。生成された3次元空間データはWebSocketを使いサーバーへリアルタイムで送信されます。 

3DヒートマップはGR-PEACHから受信した3次元空間データを基にサーバー上で生成します。なお、このプロトタイプではリアルタイム性を重視しており、DBやストレージへのデータ保存は行わずに10フレーム分の3次元空間データをサーバーのメモリ上に保持して処理しています。 

生成された3DヒートマップはPCのブラウザから見ることができます。マウス操作によって任意の視点から結果をリアルタイムにみることができます。

## サーバーの準備

2つのサーバープログラムを起動する必要があります。１つ目はGR-PEACHからの3次元データを受信してヒートマップなどを生成するWebSocketサーバーです。2つ目はデータを閲覧するためのHTMLをホスティングするWebサーバーです。

### WebSocketサーバーの起動

ヒートマップを生成するWebSocketサーバーでは [node-red](https://nodered.org/) を利用します。公式サイトの手順に従いインストールしてください。

node-redを起動したサーバーのIPアドレスを調べてメモしてください。後ほど、プログラム内にこのサーバーのIPアドレスを設定する必要があります。

以下のプログラムをnode-redにインポートしてください。

```
[{"id":"5f004c04.5e3e84","type":"websocket out","z":"2885055e.4e0d6a","name":"","server":"f2b6a0f4.e21d2","client":"","x":709.8888549804688,"y":254.44442749023438,"wires":[]},{"id":"53339c83.df18d4","type":"inject","z":"2885055e.4e0d6a","name":"","topic":"","payload":"","payloadType":"date","repeat":"","crontab":"","once":true,"x":76.44444274902344,"y":95.22222137451172,"wires":[["702670f5.6fd2c"]]},{"id":"702670f5.6fd2c","type":"function","z":"2885055e.4e0d6a","name":"初期化","func":"global.set(\"pointcloud\", {});\nglobal.set(\"_pointcloud\", {});\n\nglobal.set(\"heatmap\", {});\nglobal.set(\"heatmap_tmp\", []);\nglobal.set(\"heatmap_idx\", 0);\n\nglobal.set(\"_position\", {x:0, y:0, z:0});\nglobal.set(\"count\", 0);\n\nreturn msg;","outputs":1,"noerr":0,"x":250.4444580078125,"y":95,"wires":[["2556c1e5.badeee"]]},{"id":"2556c1e5.badeee","type":"debug","z":"2885055e.4e0d6a","name":"","active":true,"console":"false","complete":"false","x":446,"y":95,"wires":[]},{"id":"f01fd466.ab4ab8","type":"function","z":"2885055e.4e0d6a","name":"位置情報の送信","func":"delete msg._session;\nreturn msg;","outputs":1,"noerr":0,"x":484,"y":254,"wires":[["dddeab09.920ac8","5f004c04.5e3e84"]]},{"id":"dddeab09.920ac8","type":"debug","z":"2885055e.4e0d6a","name":"","active":false,"console":"false","complete":"payload","x":700,"y":203,"wires":[]},{"id":"1fc13124.44fd2f","type":"websocket in","z":"2885055e.4e0d6a","name":"","server":"4892f0e6.33965","client":"","x":95,"y":258,"wires":[["9df988db.5a4058"]]},{"id":"9df988db.5a4058","type":"switch","z":"2885055e.4e0d6a","name":"","property":"payload","propertyType":"msg","rules":[{"t":"eq","v":"end","vt":"str"},{"t":"else"}],"checkall":"true","outputs":2,"x":205,"y":350,"wires":[["f0bf0993.6cfc98","5a12776a.334078"],["2c454481.aa6edc"]]},{"id":"542141fc.11a64","type":"function","z":"2885055e.4e0d6a","name":"点群データの更新","func":"// 点群データ\nvar data = global.get(\"_pointcloud\");\n\ndata[msg.payload] = 1;\n\nglobal.set(\"_pointcloud\", data);\n\nreturn msg;","outputs":1,"noerr":0,"x":421,"y":434,"wires":[[]]},{"id":"f0bf0993.6cfc98","type":"function","z":"2885055e.4e0d6a","name":"点群データの確定","func":"var pointcloud = global.get(\"_pointcloud\");\nglobal.set(\"pointcloud\", pointcloud);\nglobal.set(\"_pointcloud\", {});\n\nreturn msg;","outputs":1,"noerr":0,"x":420,"y":343,"wires":[["19b6be51.684ba2"]]},{"id":"13a0c106.76422f","type":"debug","z":"2885055e.4e0d6a","name":"","active":false,"console":"false","complete":"payload","x":687,"y":409,"wires":[]},{"id":"933ac317.cba2a","type":"http in","z":"2885055e.4e0d6a","name":"","url":"/pointcloud","method":"get","swaggerDoc":"","x":96,"y":545,"wires":[["3e3caa06.def5d6"]]},{"id":"d30ef5ac.a321c8","type":"http response","z":"2885055e.4e0d6a","name":"","x":524,"y":580,"wires":[]},{"id":"3e3caa06.def5d6","type":"function","z":"2885055e.4e0d6a","name":"点群データの取得","func":"msg.payload = global.get(\"pointcloud\");\nmsg.headers = {\"Access-Control-Allow-Origin\": \"*\"};\n\nreturn msg;\n","outputs":1,"noerr":0,"x":315,"y":555,"wires":[["d30ef5ac.a321c8"]]},{"id":"5a12776a.334078","type":"function","z":"2885055e.4e0d6a","name":"位置情報の確定","func":"var count = global.get(\"count\");\nif (count === 0) return null;\n\nvar position = global.get(\"_position\");\nposition.x = Math.round(position.x / count);\nposition.y = Math.round(position.y / count);\nposition.z = Math.round(position.z / count);\n\nmsg.payload = position;\n\nglobal.set(\"_position\", {x:0, y:0, z:0});\nglobal.set(\"count\", 0);\n\nreturn msg;","outputs":1,"noerr":0,"x":410,"y":190,"wires":[["f01fd466.ab4ab8"]]},{"id":"2f90defe.a32522","type":"http in","z":"2885055e.4e0d6a","name":"","url":"/heatmap","method":"get","swaggerDoc":"","x":86,"y":623,"wires":[["e3b0642a.1d0f58"]]},{"id":"e697a018.2ce11","type":"http response","z":"2885055e.4e0d6a","name":"","x":517,"y":669,"wires":[]},{"id":"e3b0642a.1d0f58","type":"function","z":"2885055e.4e0d6a","name":"ヒートマップの取得","func":"msg.payload = global.get(\"heatmap\");\nmsg.headers = {\"Access-Control-Allow-Origin\": \"*\"};\n\nreturn msg;","outputs":1,"noerr":0,"x":311,"y":643,"wires":[["e697a018.2ce11"]]},{"id":"19b6be51.684ba2","type":"function","z":"2885055e.4e0d6a","name":"ヒートマップの確定","func":"var idx = global.get(\"heatmap_idx\");\nif (idx == 10) {\n    idx = 0;\n}\nelse {\n    idx++;\n}\n\nvar heatmap_tmp = global.get(\"heatmap_tmp\");\nheatmap_tmp[idx] = global.get(\"pointcloud\");\n\nglobal.set(\"heatmap_tmp\", heatmap_tmp);\nglobal.set(\"heatmap_idx\", idx);\n\n\n// ヒートマップデータの更新\nvar data = {};\nfor (var i=0;i<10;i++) {\n    for (var pos in heatmap_tmp[i]) {\n        if (pos in data) {\n            data[pos] += 1;\n        }\n        else {\n            data[pos] = 1;\n        }\n    }\n}\n\n// for (var pos in heatmap_tmp[idx]) {\n//     if (pos in data) {\n//         data[pos] += 1;\n//     }\n//     else {\n//         data[pos] = 1;\n//     }\n// }\n\n\nglobal.set(\"heatmap\", data);\n\n\nreturn msg;","outputs":1,"noerr":0,"x":626,"y":343,"wires":[["13a0c106.76422f"]]},{"id":"2c454481.aa6edc","type":"function","z":"2885055e.4e0d6a","name":"位置データの更新","func":"var str = msg.payload;\n\nvar count = global.get(\"count\");\ncount++;\nglobal.set(\"count\", count);\n\nvar position = global.get(\"_position\");\n\nposition.x += str.charCodeAt(0);\nposition.y += str.charCodeAt(1);\nposition.z += str.charCodeAt(2);\n\nglobal.set(\"_position\", position);\n\nvar ret = \"\";\nret += str.charCodeAt(0);\nret += \" \";\nret += str.charCodeAt(1);\nret += \" \";\nret += str.charCodeAt(2);\nmsg.payload = ret;\n\nreturn msg;","outputs":1,"noerr":0,"x":421,"y":386,"wires":[["542141fc.11a64"]]},{"id":"f2b6a0f4.e21d2","type":"websocket-listener","z":"","path":"/ws/position","wholemsg":"false"},{"id":"4892f0e6.33965","type":"websocket-listener","z":"","path":"/ws/pointcloud","wholemsg":"false"}]
```

### HTTPサーバーの起動

clientディレクトリ内にデータを閲覧するためのHTMLファイルがあります。index.htmlをエディタで開き、url_baseにかかれているIPアドレスをWebSocketサーバーのIPアドレスに修正してください。

```
const url_base = "192.168.0.12:1880";
```

修正後、clientディレクトリ内に移動して、Webサーバーを起動してください。サーバー起動後、ブラウザで http://localhost:8000/ にアクセスしてください。

```
$ cd client
$ python -m SimpleHTTPServer 8000
```

## HWの準備

### 回路図

特別な外付け回路は必要ありません。GR-PEACHにNTSCカメラを2台接続してください。GR-PEACHへのNTSCカメラ接続方法については、[こちらのサイト](http://gadget.renesas.com/ja/product/peach_sp4.html)を参考にしてください。

### ビルド手順

 [GR-PEACH オフライン開発環境の手順](https://os.mbed.com/users/1050186/notebook/offline-development-langja/)に従い、オフライン開発環境を構築してください。

 GitHubリポジトリからコードをインポートしてください。
```
$ mbed import https://github.com/takjn/lofsil
```

`main.cpp`をエディタで開き、以下のパラメータを環境に合わせて編集してください。

|パラメーター         |設定例                                  |設定内容                                         |
|:-----------------|:--------------------------------------|:-----------------------------------------------|
|SERVER_URL        |"ws://192.168.0.12:1880/ws/pointcloud" |IPアドレスをWebSocketサーバーのIPアドレスに修正してください。 |
|CAMERA_0_DISTANCE |3500  |1台目のカメラから中心点までの直線距離をmm単位で指定してください。|
|CAMERA_0_CENTER_U |87    |1台目のカメラのX方向の光学中心(pixel)を指定してください。|
|CAMERA_0_CENTER_V |58    |1台目のカメラのY方向の光学中心(pixel)を指定してください。|
|CAMERA_0_FX       |121.0 |1台目のカメラのX方向の焦点距離を指定してください。|
|CAMERA_0_FY       |120.0 |1台目のカメラのY方向の焦点距離を指定してください。|
|CAMERA_1_DISTANCE |3500  |2台目のカメラから中心点までの直線距離をmm単位で指定してください。|
|CAMERA_1_CENTER_U |87    |2台目のカメラのX方向の光学中心(pixel)を指定してください。|
|CAMERA_1_CENTER_V |58    |2台目のカメラのY方向の光学中心(pixel)を指定してください。|
|CAMERA_1_FX       |121.0 |2台目のカメラのX方向の焦点距離を指定してください。|
|CAMERA_1_FY       |120.0 |2台目のカメラのY方向の焦点距離を指定してください。|

以下のコマンドでビルドを行い、ビルドしたバイナリファイルをボードにコピーしてください。
```
$ mbed compile -m RZ_A1H -t GCC_ARM --profile debug
```

### 補足事項
カメラの光学中心、焦点距離を計算するためにはカメラ画像を取得して確認する必要があります。USB0にUSBケーブルを接続してPC用アプリ [DisplayApp](https://developer.mbed.org/users/dkato/code/DisplayApp/)を使うことでカメラ映像を確認することができます。

## 使用方法
### カメラの配置
このプログラムでは、幅×奥行×高さが200×200×200mmの３次元空間データを5mm間隔で取得することができます。床からカメラまでの高さは100mmとしてください。中心点とカメラまでの適切な距離はカメラの画角に依存します。以下に設定例を示します。

★TBD

### プログラムの起動

3次元データの取得処理では、背景差分法を用いて動いている人などを検知しています。GR-PEACHの電源を入れる際には、背景に人などが写り込まないようにしてください。

- GR-PEACHにLANケーブルを接続してください。
- カメラの電源を入れてください。
- GR-PEACHの電源を入れてください。この際、人などが写り込まないように注意してください。
- GR-PEACHのオンボードLED（赤）が点灯したことを確認してください。
- ブラウザで http://localhost:8000/ にアクセスしてください。
