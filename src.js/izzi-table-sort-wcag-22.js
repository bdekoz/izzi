/*
 *
 *   File:  izzi-table-sort-22.js
 *   Info:  Adds style-free sorting to a HTML data table with ARIA tags
 *   URL:   https://www.w3.org/WAI/ARIA/apg/patterns/table/examples/sortable-table/
 *   Ver:   20260506:9
 *
 *   This content is derived from
 *   Sources licensed according to the W3C Software License at
 *   https://www.w3.org/Consortium/Legal/2015/copyright-software-and-document
 */

'use strict';

class SortableTable {
  constructor(tableNode) {
    this.tableNode = tableNode;

    // Get ALL header cells in the thead (including all rows)
    this.allHeaders = tableNode.querySelectorAll('thead th');

    // Find which header cells actually contain buttons (these are the sortable columns)
    this.columnHeaders = [];
    this.sortColumns = [];

    for (var i = 0; i < this.allHeaders.length; i++) {
      var ch = this.allHeaders[i];
      var buttonNode = ch.querySelector('button');
      if (buttonNode) {
	// Store the actual DOM element and its logical column index
	this.columnHeaders.push({
	  element: ch,
	  button: buttonNode,
	  columnIndex: this.getActualColumnIndex(ch)
	});

	buttonNode.setAttribute('data-column-index', this.columnHeaders[this.columnHeaders.length - 1].columnIndex);
	buttonNode.addEventListener('click', this.handleClick.bind(this));
      }
    }
  }

  // Helper to find the actual data column index for a header cell (handles colspan)
  getActualColumnIndex(headerCell) {
    // Get the table body rows to determine column count
    var firstDataRow = this.tableNode.querySelector('tbody tr');
    if (!firstDataRow) return 0;

    // Find which column in the data row aligns with this header
    var headerRow = headerCell.parentElement;
    var headerCellsInRow = headerRow.querySelectorAll('th, td');

    var colSpanOffset = 0;
    for (var i = 0; i < headerCellsInRow.length; i++) {
      if (headerCellsInRow[i] === headerCell) {
	return colSpanOffset;
      }
      // Account for colspan in previous header cells
      var colspan = parseInt(headerCellsInRow[i].getAttribute('colspan'));
      if (!isNaN(colspan)) {
	colSpanOffset += colspan;
      } else {
	colSpanOffset += 1;
      }
    }

    return colSpanOffset;
  }

  parseNumber(str) {
    if (!str) return null;

    // Remove commas and trim whitespace
    var cleaned = str.replace(/,/g, '').trim();

    // Check if it's a valid number
    if (cleaned === '') return null;

    var num = Number(cleaned);
    if (isNaN(num)) return null;

    return num;
  }

  setColumnHeaderSort(columnIndex) {
    if (typeof columnIndex === 'string') {
      columnIndex = parseInt(columnIndex);
    }

    // Find the header element that corresponds to this column index
    var targetHeader = null;
    for (var i = 0; i < this.columnHeaders.length; i++) {
      if (this.columnHeaders[i].columnIndex === columnIndex) {
	targetHeader = this.columnHeaders[i].element;
	break;
      }
    }

    if (!targetHeader) return;

    for (var i = 0; i < this.columnHeaders.length; i++) {
      var ch = this.columnHeaders[i].element;
      var buttonNode = this.columnHeaders[i].button;

      if (ch === targetHeader) {
	var value = ch.getAttribute('aria-sort');
	if (value === 'descending') {
	  ch.setAttribute('aria-sort', 'ascending');
	  this.sortColumn(columnIndex, 'ascending');
	} else {
	  ch.setAttribute('aria-sort', 'descending');
	  this.sortColumn(columnIndex, 'descending');
	}
      } else {
	if (ch.hasAttribute('aria-sort') && buttonNode) {
	  ch.removeAttribute('aria-sort');
	}
      }
    }
  }

  sortColumn(columnIndex, sortValue) {
    // Get the main tbody (skip any thead or tfoot)
    var tbodyNode = this.tableNode.querySelector('tbody:not(.sr-only)');
    if (!tbodyNode) return;

    // Get all rows in the tbody
    var rows = Array.from(tbodyNode.children).filter(function(el) {
      return el.tagName === 'TR';
    });

    if (rows.length === 0) return;

    var asc = (sortValue === 'ascending');
    var self = this;

    // Create array of objects with row and its value
    var rowsWithValues = rows.map(function(row, idx) {
      var cell = row.cells[columnIndex];
      var rawValue = cell ? cell.innerText.trim() : '';
      var numericValue = self.parseNumber(rawValue);

      return {
	row: row,
	originalIndex: idx,
	rawValue: rawValue,
	value: numericValue !== null ? numericValue : rawValue,
	isNumeric: numericValue !== null
      };
    });

    // Sort the array
    rowsWithValues.sort(function(a, b) {
      var result = 0;

      if (a.isNumeric && b.isNumeric) {
	// Both are numbers
	result = a.value - b.value;
      } else if (!a.isNumeric && !b.isNumeric) {
	// Both are strings
	result = a.value.localeCompare(b.value);
      } else {
	// Mixed type - numbers come before strings? Actually let's treat numbers as smaller
	if (a.isNumeric) result = -1;
	else result = 1;
      }

      // Reverse for descending
      return asc ? result : -result;
    });

    // Reorder the DOM
    rowsWithValues.forEach(function(item) {
      tbodyNode.appendChild(item.row);
    });
  }

  /* EVENT HANDLERS */

  handleClick(event) {
    var tgt = event.currentTarget;
    var columnIndex = parseInt(tgt.getAttribute('data-column-index'));
    this.setColumnHeaderSort(columnIndex);
  }
}

// Initialize sortable table buttons
window.addEventListener('load', function () {
  var sortableTables = document.querySelectorAll('table.sortable');
  for (var i = 0; i < sortableTables.length; i++) {
    new SortableTable(sortableTables[i]);
  }
});
