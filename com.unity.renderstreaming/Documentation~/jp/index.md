# Unity Render Streaming

このパッケージには **Unity Render Streaming** の公開 API とサンプルプロジェクトが含まれます。

> [!Video https://www.youtube.com/embed/c2pp_T5xzeU]

## 動作環境

このテンプレートは次のバージョンの Unity エディターに対応しています。
- Unity 2019.3

> [!NOTE]
> HDRP テンプレートは `HDRP 7.3` を使用しているため、Unity2019.3 以前のバージョンでは動作しません。

| Platform    | Graphics API | Hardware Encoder                                                                                                             | Software Encoder   |
| ----------- | ------------ | ---------------------------------------------------------------------------------------------------------------------------- | ------------------ |
| Windows x64 | DirectX11    | :white_check_mark: (NVIDIA の[グラフィックスカード](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix)が必要) | :white_check_mark: |
| Windows x64 | DirectX12    | :white_check_mark: (NVIDIA の[グラフィックスカード](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix)が必要) | :white_check_mark: |
| Windows x64 | OpenGL       |                                                                                                                              |                    |
| Windows x64 | Vulkan       |                                                                                                                              |                    |
| Linux x64   | OpenGL       | :white_check_mark: (NVIDIA の[グラフィックスカード]((https://developer.nvidia.com/video-encode-decode-gpu-support-matrix)が必要) |                    |
| Linux x64   | Vulkan       |                                                                                                                              |                    |
| MacOS       | OpenGL       |                                                                                                                              |                    |
| MacOS       | Metal        |                                                                                                                              | :white_check_mark: |

> [!WARNING]
> Linux 環境と HDRP の組み合わせでの利用は、現在未対応です。Linux では Graphics API に `OpenGL` を選択した上で、レンダリングパイプラインは `Legacy` をご利用ください。

### ハードウェアエンコーダー

このパッケージでは、ハードウェアエンコーダーに **NVIDIA のグラフィックボード** を利用しています。推奨するグラフィックボード上で動作させることで、低遅延のストリーミング配信を実現することができます。ハードウェアエンコーダーが利用可能なグラフィックボードのリストについては、 NVIDIA が公開している [NVIDIA VIDEO CODEC SDK](https://developer.nvidia.com/video-encode-decode-gpu-support-matrix) のウェブページを確認してください。

### 対応ブラウザー

Unity Render Streaming は **WebRTC テクノロジー** を利用しており、 WebRTC に対応しているブラウザーと組み合わせて利用することができます。
以下のブラウザーについては動作を確認しています。

| Browser                           | Windows            | Mac                | iOS                | Android            |
| --------------------------------- | ------------------ | ------------------ | ------------------ | ------------------ |
| Google Chrome                     | :white_check_mark: | :white_check_mark: |                    | :white_check_mark: |
| Safari                            |                    | :white_check_mark: | :white_check_mark: |                    |
| Firefox                           | :white_check_mark: |                    |                    |                    |
| Microfoft edge (Chrominium based) | :white_check_mark: | :white_check_mark: |                   |                    |                    |

> [!NOTE]
> ブラウザーの WebRTC 対応状況によっては、正しく動作しない場合があります。

## サンプル

このパッケージには以下の2つのサンプルが含まれています。 Package Manager からサンプルをプロジェクトにインポートできます。

### HDRP サンプル

このサンプルをインポートすると、[HDRP](https://docs.unity3d.com/Packages/com.unity.render-pipelines.high-definition@latest) (*com.unity.render-pipelines.high-definition*) パッケージと Unity が提供している HDRP サンプルアセットのインストールと設定が自動的に行われます。

### URP サンプル

このサンプルをインポートすると、[URP](https://docs.unity3d.com/Packages/com.unity.render-pipelines.universal@latest) (*com.unity.render-pipelines.universal*) パッケージと Unity が提供している URP サンプルアセットのインストールと設定が自動的に行われます。

## その他の表示言語

- [English](../index.md)