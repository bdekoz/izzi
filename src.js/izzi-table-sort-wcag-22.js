/*
 *   This content is licensed according to the W3C Software License at
 *   https://www.w3.org/Consortium/Legal/2015/copyright-software-and-document
 *
 *   File:  sortable-table.js
 *   Desc:  Adds sorting to a HTML data table that implements ARIA Authoring Practices
 *   URL:   https://www.w3.org/WAI/ARIA/apg/patterns/table/examples/sortable-table/
 *   ver:   20260506:4
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

  // Helper function to parse numbers from strings (handles commas, decimals, etc.)
  parseNumber(str) {
    if (!str || str === '') return null;

    // Remove commas and trim whitespace
    const cleaned = str.replace(/,/g, '').trim();

    // Check if it's a valid number
    if (cleaned === '' || isNaN(cleaned)) return null;

    const num = Number(cleaned);
    return isNaN(num) ? null : num;
  }

  getValueForComparison(cell) {
    const rawValue = cell.textContent.trim();
    const numericValue = this.parseNumber(rawValue);

    return {
      raw: rawValue,
      numeric: numericValue,
      // Return the numeric value if available, otherwise the raw string (lowercase for case-insensitive compare)
      compareValue: numericValue !== null ? numericValue : rawValue.toLowerCase()
    };
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
    var rowNodes = [];
    var dataCells = [];

    var rowNode = tbodyNode.firstElementChild;

    var index = 0;
    while (rowNode) {
      rowNodes.push(rowNode);
      var rowCells = rowNode.querySelectorAll('th, td');
      var dataCell = rowCells[columnIndex];

      var comparisonInfo = this.getValueForComparison(dataCell);

      var data = {
	index: index,
	compareValue: comparisonInfo.compareValue,
	isNumeric: comparisonInfo.numeric !== null
      };

      dataCells.push(data);
      rowNode = rowNode.nextElementSibling;
      index += 1;
    }

    // Sort using the comparison values
    var self = this;
    dataCells.sort(function(a, b) {
      var aVal = a.compareValue;
      var bVal = b.compareValue;

      if (sortValue === 'ascending') {
	if (typeof aVal === 'number' && typeof bVal === 'number') {
	  return aVal - bVal;
	} else {
	  // Convert to strings for localeCompare if they're not both numbers
	  return String(aVal).localeCompare(String(bVal));
	}
      } else {
	if (typeof aVal === 'number' && typeof bVal === 'number') {
	  return bVal - aVal;
	} else {
	  return String(bVal).localeCompare(String(aVal));
	}
      }
    });

    // remove rows
    while (tbodyNode.firstChild) {
      tbodyNode.removeChild(tbodyNode.lastChild);
    }

    // add sorted rows
    for (var i = 0; i < dataCells.length; i += 1) {
      tbodyNode.appendChild(rowNodes[dataCells[i].index]);
    }
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
