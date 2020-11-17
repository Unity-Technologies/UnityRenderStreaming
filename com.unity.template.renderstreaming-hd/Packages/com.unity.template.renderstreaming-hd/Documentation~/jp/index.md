# Unity Render Streaming HDRP テンプレート

**Unity Render Streaming** は、ブラウザー上で Unity の高品位なレンダリングを実現するためのソリューションです。カーコンフィギュレーターや建築モデルをスマートデバイスで閲覧するニーズを想定しています。

このパッケージは、HDRP のシーンに対して Unity Render Streaming の利用に必要な設定を加えたテンプレートパッケージです。すぐに試してみたい方は[チュートリアルドキュメント](tutorial.md)をご覧ください。より技術的な詳細は Unity Render Streaming の[パッケージドキュメント](https://docs.unity3d.com/Packages/com.unity.renderstreaming@latest)をご覧ください。

![Browser HDRP scene](../images/browser_hdrpscene.png)

ストリーミング技術には [WebRTC](https://webrtc.org/) というテクノロジを用いて実現しています。このソリューションをカスタマイズすることで、様々なソリューションを構築することができます。

> [!NOTE]
> 既存のプロジェクトに対してストリーミング機能を追加する場合は、テンプレートではなく、Package Manager から `Unity Render Streaming` パッケージをインストールしてください。
> 詳細は[パッケージのドキュメント](https://docs.unity3d.com/Packages/com.unity.renderstreaming@latest)を確認してください。

## 動作環境

このテンプレートは次のバージョンの Unity エディターに対応しています。
- Unity 2019.4

> [!NOTE]
> このテンプレートは `HDRP 7.3` を使用しているため、古い Unity バージョンでは動作しません。

> [!WARNING]
> Linux 環境と HDRP の組み合わせでの利用は、現在未対応です。Linux では Graphics API に `OpenGL` を選択した上で、レンダリングパイプラインは `Legacy` をご利用ください。

### ハードウェアエンコーダー

このパッケージでは、ハードウェアエンコーダーに **NVIDIA のグラフィックボード** を利用しています。推奨するグラフィックボード上で動作させることで、低遅延のストリーミング配信を実現することができます。推奨グラフィックボードのリストについては、 NVIDIA が公開している [NVIDIA VIDEO CODEC SDK](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix) のウェブページを確認してください。

### 対応ブラウザー

Unity Render Streaming は **WebRTC テクノロジー** を利用しており、 WebRTC に対応しているブラウザーと組み合わせて利用することができます。
以下のブラウザーについては動作を確認しています。

| Browser                           | Windows            | Mac                | iOS                | Android            |
| --------------------------------- | ------------------ | ------------------ | ------------------ | ------------------ |
| Google Chrome                     | :white_check_mark: | :white_check_mark: |                    | :white_check_mark: |
| Safari                            |                    | :white_check_mark: | :white_check_mark: |                    |
| Firefox                           | :white_check_mark: |                    |                    |                    |
| Microsoft edge (Chromium based)   | :white_check_mark: |                    |                    |                    |

> [!NOTE]
> ブラウザーの WebRTC 対応状況によっては、正しく動作しない場合があります。

## その他の表示言語

- [English](../index.md)