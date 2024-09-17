import pandas as pd
import plotly.figure_factory as ff

intervals = []

colors = ['#aaaaff' for i in range(200)]

with open("results.txt", 'r') as f:
    start_times = {}
    t = 0
    for ln in f.readlines():
        ln_parts = ln.split()
        if ln_parts[0] == "MALLOC":
            start_times[ln_parts[1]] = t
            t = t + 1
        elif ln_parts[0] == "FREE":
            mem = ln_parts[1]
            intervals = intervals + [dict(Start=t, Finish=start_times[mem], Task=mem, Resource=mem)]
            t = t + 1

fig = ff.create_gantt(intervals, index_col='Resource', colors=colors, group_tasks=True)
fig.update_layout(xaxis_type='linear', autosize=False, width=800, height=400)
fig.show()
