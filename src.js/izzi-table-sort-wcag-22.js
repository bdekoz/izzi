/*
 *   File:  izzi-table-stort-wcag-22.js
 *   Desc:  Adds sorting to a HTML data table that implements ARIA Authoring Practices
 *   URL:   https://www.w3.org/WAI/ARIA/apg/patterns/table/examples/sortable-table/
 *   ver:   20260506:6
 *
 *   This content is derived from sources originally licensed according to:
 *   the W3C Software License at
 *   https://www.w3.org/Consortium/Legal/2015/copyright-software-and-document
 *
 */

'use strict';

class SortableTable {
  constructor(tableNode) {
    this.tableNode = tableNode;

    this.columnHeaders = tableNode.querySelectorAll('thead th');

    this.sortColumns = [];

    for (var i = 0; i < this.columnHeaders.length; i++) {
      var ch = this.columnHeaders[i];
      var buttonNode = ch.querySelector('button');
      if (buttonNode) {
	this.sortColumns.push(i);
	buttonNode.setAttribute('data-column-index', i);
	buttonNode.addEventListener('click', this.handleClick.bind(this));
      }
    }

    this.optionCheckbox = document.querySelector(
      'input[type="checkbox"][value="show-unsorted-icon"]'
    );

    if (this.optionCheckbox) {
      this.optionCheckbox.addEventListener(
	'change',
	this.handleOptionChange.bind(this)
      );
      if (this.optionCheckbox.checked) {
	this.tableNode.classList.add('show-unsorted-icon');
      }
    }
  }

  // Exact copy of the working parseNumber function from izzi-table-sort-inline.js
  parseNumber(str) {
    if (!str) return null;

    // Remove commas and trim whitespace
    const cleaned = str.replace(/,/g, '').trim();

    // Check if it's a valid number
    if (cleaned === '' || isNaN(cleaned)) return null;

    const num = Number(cleaned);
    return isNaN(num) ? null : num;
  }

  setColumnHeaderSort(columnIndex) {
    if (typeof columnIndex === 'string') {
      columnIndex = parseInt(columnIndex);
    }

    for (var i = 0; i < this.columnHeaders.length; i++) {
      var ch = this.columnHeaders[i];
      var buttonNode = ch.querySelector('button');
      if (i === columnIndex) {
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
    var tbodyNode = this.tableNode.querySelector('tbody');
    var rows = Array.from(tbodyNode.rows);

    // Convert sortValue to boolean asc (true for ascending, false for descending)
    var asc = (sortValue === 'ascending');

    var self = this;
    rows.sort(function(a, b) {
      // Get cell values - use innerText like the working version
      var x = a.cells[columnIndex].innerText.trim();
      var y = b.cells[columnIndex].innerText.trim();

      // Check if both values are numbers (including formatted numbers with commas)
      var xNum = self.parseNumber(x);
      var yNum = self.parseNumber(y);

      if (xNum !== null && yNum !== null) {
	// Both are numbers - sort numerically
	return asc ? xNum - yNum : yNum - xNum;
      } else {
	// At least one is text - sort as strings
	return asc ? x.localeCompare(y) : y.localeCompare(x);
      }
    });

    // Re-append sorted rows
    rows.forEach(function(row) {
      tbodyNode.appendChild(row);
    });
  }

  /* EVENT HANDLERS */

  handleClick(event) {
    var tgt = event.currentTarget;
    this.setColumnHeaderSort(tgt.getAttribute('data-column-index'));
  }

  handleOptionChange(event) {
    var tgt = event.currentTarget;

    if (tgt.checked) {
      this.tableNode.classList.add('show-unsorted-icon');
    } else {
      this.tableNode.classList.remove('show-unsorted-icon');
    }
  }
}

// Initialize sortable table buttons
window.addEventListener('load', function () {
  var sortableTables = document.querySelectorAll('table.sortable');
  for (var i = 0; i < sortableTables.length; i++) {
    new SortableTable(sortableTables[i]);
  }
});
