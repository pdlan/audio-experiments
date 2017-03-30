audio-tool
==========
一些简单的音频处理实验

重采样
------
简单的线性重采样

变调
----
简单的变调程序。处理方法：对音频分块FFT，伸缩频域后IFFT。可使用多种窗函数，但默认使用矩形窗。升调效果尚可，但降调效果不理想。

示例：アイの庭 [处理前(soundcloud)](https://soundcloud.com/nw68p53359y2/ainoniwa) [处理后(soundcloud)](https://soundcloud.com/nw68p53359y2/ainoniwa-shifted)
[处理前(百度盘)](https://pan.baidu.com/s/1i48mpVn) [处理后(百度盘)](https://pan.baidu.com/s/1c1M96E0)