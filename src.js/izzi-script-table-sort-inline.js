// claude sonnet 4.6
// 2026-03-01
/*
<table id="myTable">
    <thead>
    <tr><th onclick="sortTable(0)">A</th><th>B</th></tr>
    </thead>
  <tbody>
    <tr><td>Banana</td><td>2</td></tr>
    <tr><td>Apple</td><td>1</td></tr>
  </tbody>
</table>
*/

function sortTable(colIndex) {
  const table = document.getElementById("myTable");
  const rows = Array.from(table.tBodies[0].rows);
  const asc = table.dataset.sortAsc !== "true";
  table.dataset.sortAsc = asc;

  rows.sort((a, b) => {
    const x = a.cells[colIndex].innerText;
    const y = b.cells[colIndex].innerText;
    return asc ? x.localeCompare(y) : y.localeCompare(x);
  });

  rows.forEach(row => table.tBodies[0].appendChild(row));
}
