export function plotData(data) {
  const trace = {
    x: data.x,
    y: data.y,
    type: "scatter",
    mode: "lines+markers",
    name: "Jeff's Data"
  };

  Plotly.newPlot("plot", [trace]);
}
