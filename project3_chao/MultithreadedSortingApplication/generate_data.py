import numpy as np
data=np.random.randint(0,int(1e6),size=int(1e4))
with open('data.txt','w') as f:    #设置文件对象
    for i in range(int(1e4)):
        f.write(str(data[i])+"\n")