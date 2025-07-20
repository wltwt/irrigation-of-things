export function plotDashboard(data) {
  const order  = ["probe1", "probe2", "humidity", "temperature"];
  const series = order.flatMap(name => data.series.filter(s => s.name === name));
  const rows   = series.length;


  const traces = series.map((s, i) => ({
    x: s.timestamps.map(t => new Date(t)),
    y: s.values,
    mode: "lines",
    name: s.name,
    yaxis: "y" + (i ? i + 1 : ""),
    xaxis: "x" + (i ? i + 1 : "")
  }));

  const layout = {
    grid: {
      rows,
      columns: 1,
      pattern: "independent",
      roworder: "top to bottom",
      vertical_spacing: 0.04
    },
    legend: {
      orientation: "h",
      y: -0.25
    },
    margin: {t: 20, b: 50, r: 20, l: 60}
  };

  for (let r = 1; r <= rows; r++) {
    const ax = "xaxis" + (r === 1 ? "" : r);
    if (r < rows) {
      layout[ax] = {matches: "x", showticklabels: false, showgrid: false};
    } else {
      layout[ax] = {title: "Dato"};
    }
  }

  series.forEach((s, idx) => {
    layout["yaxis" + (idx ? idx + 1 : "")] = {title: s.unit || ""};
  });

  const bottomId = rows === 1 ? "x" : "x" + rows;

  for (let r = 1; r < rows; r++) {
    const ax = "xaxis" + (r === 1 ? "" : r);
    layout[ax] = {
      matches: bottomId,
      showticklabels: false,
      showgrid: false
    };
  }


  const bottomAxisKey = "xaxis" + (rows === 1 ? "" : rows);
  layout[bottomAxisKey] = {title: "Dato"};


  Plotly.newPlot("plot", traces, layout, {responsive: true});
}