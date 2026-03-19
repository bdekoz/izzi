// claude sonnet 4.6
// 2026-03-01
<script src="https://unpkg.com/tabular/dist/tabular.min.js"></script>

<script>
  document.addEventListener('DOMContentLoaded', function() {
    // Select all table elements on the page
    const tables = document.querySelectorAll('table');

    // Apply Tabular to each table found
    tables.forEach(table => {
      // Basic initialization for sortable tables.
      // You might need to adjust options based on your table structure (e.g., header rows).
      new Tabular(table, {
        // 'dataTabular' makes the table sortable by clicking on headers.
        // It assumes the first row (<tr>) contains the header cells (<th>).
        dataTabular: true
      });
    });
  });
</script>
