audio-tool
==========
一些简单的音频处理实验

重采样
------
简单的线性重采样

变调
----
简单的变调程序。处理方法：对音频分块FFT，伸缩频域后IFFT。升调效果尚可，但降调效果不理想。可使用多种窗函数，但经过多次试验，发现大小为4096矩形窗效果最佳。

使用方法: ./audio-tool shift-pitch in.wav out.wav ratio [window-type=rect] [window-size=4096]

其中ratio是变调比率，大于1为升调，小于1为降调，上升n个半音比率为2^(n/12)。

示例：
- アイの庭(升调，比率为1.2) [处理前(soundcloud)](https://soundcloud.com/nw68p53359y2/ainoniwa) [处理后(soundcloud)](https://soundcloud.com/nw68p53359y2/ainoniwa-shifted) [处理前(百度盘)](https://pan.baidu.com/s/1i48mpVn) [处理后(百度盘)](https://pan.baidu.com/s/1c1M96E0)
- 白金(升调，比率为1.2) [处理前(百度盘)](https://pan.baidu.com/s/1nuNDprV) [处理后(百度盘)](https://pan.baidu.com/s/1qY4kAIg)
- 勃兰登堡协奏曲第三首第三乐章(降调，比率为0.9) [处理前(百度盘)](https://pan.baidu.com/s/1b3GYcQ) [处理后(百度盘)](https://pan.baidu.com/s/1pL6o4Oz)
- 德沃夏克Mesicku na nebi hlubokem(降调，比率为0.8) [处理前(百度盘)](https://pan.baidu.com/s/1kVypg4B) [处理后(百度盘)](https://pan.baidu.com/s/1sl8SnoD)

滤波
----
简单的FFT滤波器。通过配置文件用FFT滤波。配置文件格式：第一行为使用的窗函数与窗口大小(N)，用空格分隔。其后为FFT各项相乘的系数，从第0项到第N/2项(奈奎斯特频率)，用空格分隔。也可使用./audio-tool generate-fft-filter-profile-lowpass等生成配置。

窗函数
------
可用的窗函数有rect，hann，blackman。大小均默认为4096。