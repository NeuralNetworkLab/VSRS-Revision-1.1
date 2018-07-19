## 描述
该项目是修改版的 VSRS 软件，通过两视点的原始图片和深度图片来生成的空洞图。
[PreVersion](https://github.com/keifergu/vsrs)

## 使用

在 [MPEG-FTV](http://www.fujii.nuee.nagoya-u.ac.jp/multiview-data/mpeg/mpeg_ftv.html)下载视频流资源和对应的配置文件，在windows文件夹中打开对应版本的解决方案，编译运行代码。
PictureSouce 文件夹放置 cfg参数文件，yuv视频文件，yuv深度文件。
默认生成的exe文件在bin文件夹中，默认的空洞图生成在ViewSyn项目文件夹下

## 注意
当前此项目只能处理[IYUV(I420)](http://www.fourcc.org/pixel-format/yuv-i420/)格式的图像，后续可能会支持更多。