import numpy as np
import plotly.graph_objects as go

# 读取频谱数据
data = np.genfromtxt('amplitude.txt', delimiter=',')
with open('phase.txt', 'r') as file:
    phase = float(file.read())

# 生成时间序列
t = np.linspace(0, 1, 1000)
f = 0

# 创建频谱图
x = np.arange(len(data))  # 使用 0 到 15 作为横坐标
fig = go.Figure()
fig.add_trace(go.Scatter(x=x, y=data))

# 寻找非零值的位置，并绘制垂直线
for i, y_value in enumerate(data):
    if y_value != 0:
        fig.add_shape(
            type='line',
            x0=x[i], y0=0,
            x1=x[i], y1=y_value,
            line=dict(color='red', width=2)
        )
        if i < len(data)/2:
            f = i
    if i == len(data) // 2:  # 使用双等号进行相等比较
        x_avg = (x[i-1] + x[i]) / 2  # 计算相邻索引的平均值
        fig.add_shape(
            type='line',
            x0=x_avg, y0=0,
            x1=x_avg, y1=1.5,
            line=dict(color='blue', width=1)
        )
fig.update_layout(
    xaxis=dict(title='Frequency/Hz', dtick=1),
    yaxis=dict(title='Amplitude'),
    title='Frequency Spectrum',
    xaxis_range=[0, len(data)-1]  # 设置x轴范围从0到最大值
)

# 绘制原始信号图
x1 = (2 * np.pi * f * t + phase).tolist()
y1 = np.cos(x1)
fig_cosx = go.Figure(data=go.Scatter(x=t, y=y1))

# 设置图表布局
fig_cosx.update_layout(
    xaxis_title='Time/s',
    yaxis_title='Amplitude',
    title='Original signal f(t)',
    xaxis_range=[0, 1]  # 设置x轴范围从0到1
)

# 在图像上显示表达式
expression = r'$f(t) = \cos({} \cdot t + \text{{phase}})$'.format(round(2 * np.pi * f, 2))
expression = expression.replace('phase', str(phase))
fig_cosx.add_annotation(
    x=0.5, y=1.2,
    text=expression,
    showarrow=False,
    font=dict(size=14)
)

fig.update_traces(line=dict(color='rgba(0,0,0,0)'))  # 设置折线颜色为无色
fig.show()
fig_cosx.show()