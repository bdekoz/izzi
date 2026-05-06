/*
 *   This content is licensed according to the W3C Software License at
 *   https://www.w3.org/Consortium/Legal/2015/copyright-software-and-document
 *
 *   File:  sortable-table.js
 *   Desc:  Adds sorting to a HTML data table that implements ARIA Authoring Practices
 *   URL:   https://www.w3.org/WAI/ARIA/apg/patterns/table/examples/sortable-table/
 *   ver:   20260506:5
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

  // Fixed: Proper number parsing that actually works
  getNumericValue(str) {
    if (!str || str === '') return null;
    
    // Remove commas and trim
    var cleaned = str.replace(/,/g, '').trim();
    
    // Try to convert to number
    var num = parseFloat(cleaned);
    
    // Check if it's a valid number (not NaN and finite)
    if (!isNaN(num) && isFinite(num)) {
      return num;
    }
    
    return null;
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
      
      // Get the raw text and try to parse as number
      var rawValue = dataCell.textContent.trim();
      var numericValue = this.getNumericValue(rawValue);
      
      var data = {
        index: index,
        rawValue: rawValue,
        numericValue: numericValue,
        isNumeric: numericValue !== null
      };
      
      dataCells.push(data);
      rowNode = rowNode.nextElementSibling;
      index += 1;
    }

    // Sort the data
    var self = this;
    dataCells.sort(function(a, b) {
      var result = 0;
      
      // Check if both values are numeric
      if (a.isNumeric && b.isNumeric) {
        // Numeric comparison
        if (sortValue === 'ascending') {
          result = a.numericValue - b.numericValue;
        } else {
          result = b.numericValue - a.numericValue;
        }
      } else {
        // String comparison - use lower case for case-insensitive
        var aStr = a.rawValue.toLowerCase();
        var bStr = b.rawValue.toLowerCase();
        
        if (sortValue === 'ascending') {
          result = aStr.localeCompare(bStr);
        } else {
          result = bStr.localeCompare(aStr);
        }
      }
      
      return result;
    });

    // Remove all rows
    while (tbodyNode.firstChild) {
      tbodyNode.removeChild(tbodyNode.firstChild);
    }

    // Append sorted rows
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
