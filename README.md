audio-tool
==========
һЩ�򵥵���Ƶ����ʵ��

�ز���
------
�򵥵������ز���

���
----
�򵥵ı�����򡣴�����������Ƶ�ֿ�FFT������Ƶ���IFFT������Ч���пɣ�������Ч�������롣��ʹ�ö��ִ�������������������飬���ִ�СΪ4096���δ�Ч����ѡ�
ʹ�÷���: ./audio-tool shift-pitch in.wav out.wav ratio [window-type=rect] [window-size=4096]
����ratio�Ǳ�����ʣ�����1Ϊ������С��1Ϊ����������n����������Ϊ2^(n/12)��
ʾ����
- ������ͥ(����������Ϊ1.2) [����ǰ(soundcloud)](https://soundcloud.com/nw68p53359y2/ainoniwa) [�����(soundcloud)](https://soundcloud.com/nw68p53359y2/ainoniwa-shifted) [����ǰ(�ٶ���)](https://pan.baidu.com/s/1i48mpVn) [�����(�ٶ���)](https://pan.baidu.com/s/1c1M96E0)
- �׽�(����������Ϊ1.2) [����ǰ(�ٶ���)](https://pan.baidu.com/s/1nuNDprV) [�����(�ٶ���)](https://pan.baidu.com/s/1qY4kAIg)

�˲�
----
�򵥵�FFT�˲�����ͨ�������ļ���FFT�˲��������ļ���ʽ����һ��Ϊʹ�õĴ������봰�ڴ�С(N)���ÿո�ָ������ΪFFT������˵�ϵ�����ӵ�0���N/2��(�ο�˹��Ƶ��)���ÿո�ָ���Ҳ��ʹ��./audio-tool generate-fft-filter-profile-lowpass���������á�

������
------
���õĴ�������rect��hann��blackman����С��Ĭ��Ϊ4096��