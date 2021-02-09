# AR Foundation sample

AR Foundation サンプルは、 AR Foundation がサポートするデバイスを利用して、別 PC 上のカメラの位置や向きを操作するためのサンプルです。

[!NOTE] このパッケージでは iOS デバイスの動作を確認しています。Android やその他のデバイスでの動作確認はしていません。

[!NOTE] 送信側と受信側の双方で利用可能なコーデックが存在しない場合、映像がストリームされません。コーデックの対応状況がプラットフォームとグラフィック API によって異なることに注意してください。

## サポートするバージョン

AR Foundation の検証バージョンが異なります。

| Unity version | scene name    |
| ------------- | ------------- |
| Unity2019.4   | `ARFoundation2.1` |
| Unity2020.2   | `ARFoundation4.0` |

## 利用方法

1. シグナリングサーバを `public` モードで起動します。起動方法は `Web application` を参照。

2. 依存パッケージをインストールします。iOS の場合は `ARKit 2.1.14`、`AR Foundation 2.1.14` をインストールします。

3. iOS の場合、`Player Settings` で `Requires ARKit support` を有効にします。また `Architecture` は `ARM64` を選択する必要があります。

![Player Settings](images/playersettings_arfoundation.png)

4. `ARFoundation2.1` もしくは `ARFoundation4.0` シーンを開き、インスペクタ上でシグナリングサーバの IP アドレスを指定します。

5. ビルドプラットフォームを指定（現在は iOS のみ）してビルドします。PC にデバイスを接続し、デバイスにアプリケーションを転送します。

6. `Broadcast` シーンを開き、インスペクタ上でシグナリングサーバの IP アドレスを指定します。

7. `Broadcast` シーンを実行します。

8. デバイス上の Play ボタンを押して映像が転送されていることを確認します。