import matplotlib.pyplot as plt
import csv
  
x = []
y = []
  
with open('taskA_1.csv','r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    for row in lines:
        x.append(row[0])
        y.append(float(row[2]))
  
plt.plot(x, y, color = 'g', linestyle = 'dashed',
         marker = 'o',label = "")
  
plt.xticks(rotation = 25)
plt.xlabel('Packets per second')
plt.ylabel('End-to-end Delay (s)')
plt.title('Wireless high-rate (static)', fontsize = 20)
plt.grid()
plt.legend()
fig=plt.gcf()
plt.show()
fig.savefig("All_Graph.pdf")
