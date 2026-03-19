// claude sonnet 4.6
// 2026-03-01
/*
<table id="myTable">
    <thead>
      <tr>
	<th onclick="sortTable(0)">A</th><th>B</th>
      </tr>
    </thead>
  <tbody>
    <tr><td>Banana</td><td>2</td></tr>
    <tr><td>Apple</td><td>1</td></tr>
  </tbody>
</table>
*/

function sortTable(tID, colIndex) {
  const table = document.getElementById(tID);
  const rows = Array.from(table.tBodies[0].rows);
  const asc = table.dataset.sortAsc !== "true";
  table.dataset.sortAsc = asc;

  rows.sort((a, b) => {
    const x = a.cells[colIndex].innerText.trim();
    const y = b.cells[colIndex].innerText.trim();

    // Check if both values are numbers (including formatted numbers with commas)
    const xNum = parseNumber(x);
    const yNum = parseNumber(y);

    if (xNum !== null && yNum !== null) {
      // Both are numbers - sort numerically
      return asc ? xNum - yNum : yNum - xNum;
    } else {
      // At least one is text - sort as strings
      return asc ? x.localeCompare(y) : y.localeCompare(x);
    }
  });

  rows.forEach(row => table.tBodies[0].appendChild(row));
}

// Helper function to parse numbers from strings (handles commas, decimals, etc.)
function parseNumber(str) {
  if (!str) return null;

  // Remove commas and trim whitespace
  const cleaned = str.replace(/,/g, '').trim();

  // Check if it's a valid number
  if (cleaned === '' || isNaN(cleaned)) return null;

  const num = Number(cleaned);
  return isNaN(num) ? null : num;
}
