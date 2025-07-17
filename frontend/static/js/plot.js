export function plotData(data) {
  const trace = {
    x: data.timestamps.map(t => new Date(t)),
    y: data.values,
    type: "scatter",
    mode: "lines+markers",
    name: "Soil Temp"
  };

  Plotly.newPlot("plot", [trace]);
}
