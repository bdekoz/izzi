// baseline-v7 - Function-generated geojson map
function leaflet_map_geojson(geojsonUrl) {
    // Extract title from URL (remove "cumulative.geojson" and clean up)
    function getTitleFromUrl(url) {
	const fileName = url.split('/').pop();
	const title = fileName.replace('-cumulative.geojson', '').replace('cumulative.geojson', '');
	return title.split(/[-_]/).map(word =>
	    word.charAt(0).toUpperCase() + word.slice(1)
	).join(' ');
    }

    const pageTitle = getTitleFromUrl(geojsonUrl);

    return `<!DOCTYPE html>
<html>
<head>
    <title>${pageTitle}</title>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" />
    <style>
	#map { height: 100vh; }
	.control-panel {
	    position: absolute;
	    top: 10px;
	    right: 10px;
	    z-index: 1000;
	    background: white;
	    padding: 15px;
	    border-radius: 5px;
	    box-shadow: 0 0 15px rgba(0,0,0,0.2);
	    max-width: 320px;
	    max-height: 80vh;
	    overflow-y: auto;
	}
	.legend-circle {
	    border-radius: 50%;
	    background: #3388ff;
	    display: inline-block;
	    margin-right: 8px;
	    opacity: 0.7;
	}
	.legend-item {
	    margin: 8px 0;
	    display: flex;
	    align-items: center;
	}
	.property-selector {
	    margin: 15px 0;
	    padding: 10px;
	    background: #f5f5f5;
	    border-radius: 5px;
	}
	.property-selector select {
	    width: 100%;
	    padding: 8px;
	    margin-top: 5px;
	    border-radius: 3px;
	    border: 1px solid #ccc;
	}
	.category-selector {
	    margin: 10px 0;
	    display: flex;
	    gap: 10px;
	}
	.category-btn {
	    flex: 1;
	    padding: 8px;
	    border: 1px solid #3388ff;
	    background: white;
	    color: #3388ff;
	    border-radius: 3px;
	    cursor: pointer;
	    font-weight: bold;
	}
	.category-btn.active {
	    background: #3388ff;
	    color: white;
	}
	.reduction-info {
	    font-size: 0.9em;
	    color: #666;
	    margin-top: 15px;
	    padding: 10px;
	    background: #f0f7ff;
	    border-radius: 5px;
	    border-left: 3px solid #3388ff;
	}
	button {
	    background: #3388ff;
	    color: white;
	    border: none;
	    padding: 10px 12px;
	    border-radius: 3px;
	    cursor: pointer;
	    width: 100%;
	    margin: 10px 0;
	    font-weight: bold;
	}
	button:hover {
	    background: #2868c7;
	}
	button.secondary {
	    background: #6c757d;
	}
	button.secondary:hover {
	    background: #5a6268;
	}
	.slider-container {
	    margin: 20px 0;
	    padding: 10px;
	    background: #f9f9f9;
	    border-radius: 5px;
	}
	.slider-container label {
	    display: block;
	    margin-bottom: 10px;
	    font-weight: bold;
	}
	.slider-container input {
	    width: 100%;
	    margin: 5px 0;
	}
	.slider-value {
	    text-align: center;
	    font-size: 1.2em;
	    font-weight: bold;
	    color: #3388ff;
	    margin: 5px 0;
	}
	.config-section {
	    margin: 15px 0;
	    padding: 10px;
	    background: #e9ecef;
	    border-radius: 5px;
	}
	.config-section h4 {
	    margin: 0 0 10px 0;
	    color: #495057;
	}
	.config-row {
	    display: flex;
	    align-items: center;
	    margin: 8px 0;
	}
	.config-row label {
	    width: 100px;
	    font-size: 0.9em;
	}
	.config-row input {
	    width: 80px;
	    padding: 4px;
	    border: 1px solid #ced4da;
	    border-radius: 3px;
	}
	.config-row span {
	    margin-left: 8px;
	    font-size: 0.9em;
	    color: #6c757d;
	}
	.note {
	    font-size: 0.8em;
	    color: #888;
	    margin-top: 10px;
	    font-style: italic;
	}
	.warning-info {
	    font-size: 0.9em;
	    color: #856404;
	    background: #fff3cd;
	    padding: 10px;
	    border-radius: 5px;
	    margin: 10px 0;
	    border-left: 3px solid #ffc107;
	}
	.success-info {
	    font-size: 0.9em;
	    color: #155724;
	    background: #d4edda;
	    padding: 10px;
	    border-radius: 5px;
	    margin: 10px 0;
	    border-left: 3px solid #28a745;
	}
	.data-source {
	    font-size: 0.8em;
	    color: #6c757d;
	    margin-top: 5px;
	    padding: 5px;
	    background: #f8f9fa;
	    border-radius: 3px;
	    word-break: break-all;
	}
    </style>
</head>
<body>
    <div id="map"></div>
    <div class="control-panel">
	<h3 id="panel-title">${pageTitle}</h3>

	<div class="category-selector">
	    <button id="category-downloaders" class="category-btn active">Downloaders</button>
	    <button id="category-uploaders" class="category-btn">Uploaders</button>
	</div>

	<div class="property-selector">
	    <label for="property-select"><strong>Visualize by:</strong></label>
	    <select id="property-select">
		<option value="size">size (total)</option>
		<option value="mobile">mobile</option>
		<option value="satellite">satellite</option>
		<option value="tor">tor</option>
		<option value="tor_exit_nodes">tor_exit_nodes</option>
		<option value="vpn">vpn</option>
		<option value="relay">relay</option>
		<option value="proxy">proxy</option>
		<option value="hosting">hosting</option>
		<option value="service">service</option>
	    </select>
	</div>

	<div id="status-message"></div>

	<div class="slider-container">
	    <label for="distance-slider"><strong>Max Merge Distance:</strong></label>
	    <input type="range" id="distance-slider" min="1" max="500" value="250" step="1">
	    <div class="slider-value" id="distance-value">250 km</div>
	    <p style="font-size: 0.8em; margin: 5px 0;">Points within this distance will be merged to prevent overlap (1-500km)</p>
	</div>

	<div class="config-section">
	    <h4>Circle Style</h4>
	    <div class="config-row">
		<label for="min-radius">Min radius:</label>
		<input type="number" id="min-radius" min="1" max="50" value="5" step="1">
		<span>pixels</span>
	    </div>
	    <div class="config-row">
		<label for="max-radius">Max radius:</label>
		<input type="number" id="max-radius" min="10" max="200" value="100" step="1">
		<span>pixels</span>
	    </div>
	    <div class="config-row">
		<label for="fill-opacity">Fill opacity:</label>
		<input type="number" id="fill-opacity" min="0" max="100" value="50" step="5">
		<span>%</span>
	    </div>
	    <button id="apply-style" class="secondary" style="margin: 5px 0 0;">Apply Style</button>
	</div>

	<button id="apply-reduction">Update Map</button>
	<button id="reset-view" class="secondary">Reset View</button>

	<div id="legend">
	    <h4>Legend</h4>
	    <div id="legend-content"></div>
	</div>

	<div class="reduction-info" id="reduction-info">
	    <strong>Statistics</strong>
	    <p id="point-counts">Original: 0 | Filtered: 0 | Current: 0</p>
	    <p id="reduction-percent">Reduction: 0%</p>
	    <p id="merge-distance">Merge distance: 0 km</p>
	    <p id="value-range">Value range: 0 - 0</p>
	    <p id="zero-count">Zero values: 0 points hidden</p>
	</div>

	<div class="data-source" id="data-source"></div>

	<div class="note">Click circles for details • Zero values are hidden • 0.5pt stroke</div>
    </div>

    <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>

    <script>
	// GeoJSON data source URL (passed in)
	const GEOJSON_URL = '${geojsonUrl}';

	// Default settings
	const DEFAULT_CATEGORY = 'downloaders';
	const DEFAULT_PROPERTY = 'size';

	// Map default view
	const DEFAULT_MAP_CENTER = [20, 0];
	const DEFAULT_MAP_ZOOM = 2;

	// =============================================
	// END CONFIGURABLE VARIABLES
	// =============================================

	// Initialize the map
	const map = L.map('map').setView(DEFAULT_MAP_CENTER, DEFAULT_MAP_ZOOM);

	// Add OpenStreetMap base layer
	L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
	    attribution: '© <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
	}).addTo(map);

	// Display data source
	document.getElementById('data-source').innerHTML = \`📁 <strong>Data:</strong> \${GEOJSON_URL.split('/').pop()}\`;

	// Configuration
	let config = {
	    minRadius: 5,
	    maxRadius: 100,
	    fillOpacity: 0.5
	};

	let geoJsonData = null;
	let currentLayer = null;
	let currentCategory = DEFAULT_CATEGORY;
	let currentProperty = DEFAULT_PROPERTY;
	let originalFeatures = [];
	let currentReducedPoints = [];

	// Helper function to parse JSON strings and get nested property value
	function getPropertyValue(feature, category, property) {
	    try {
		const jsonString = feature.properties[category];
		if (!jsonString) return 0;

		const parsed = JSON.parse(jsonString);
		const val = parsed[property];
		return val !== undefined ? parseFloat(val) : 0;
	    } catch (e) {
		console.warn(\`Error parsing \${category} JSON for property \${property}:\`, e);
		return 0;
	    }
	}

	// Helper functions
	function deg2km(lat) {
	    const latKm = 111.32;
	    const lngKm = 111.32 * Math.cos(lat * Math.PI / 180);
	    return { latKm, lngKm };
	}

	function pixelsToKm(pixels, lat, zoom) {
	    const resolution = (156543.03392 * Math.cos(lat * Math.PI / 180)) / Math.pow(2, zoom);
	    return (pixels * resolution) / 1000;
	}

	function calculateDistance(point1, point2) {
	    const [lng1, lat1] = point1.geometry ?
		[point1.geometry.coordinates[0], point1.geometry.coordinates[1]] :
		[point1.lng, point1.lat];
	    const [lng2, lat2] = point2.geometry ?
		[point2.geometry.coordinates[0], point2.geometry.coordinates[1]] :
		[point2.lng, point2.lat];

	    const { latKm, lngKm } = deg2km((lat1 + lat2) / 2);

	    const dLat = (lat2 - lat1) * latKm;
	    const dLng = (lng2 - lng1) * lngKm;

	    return Math.sqrt(dLat * dLat + dLng * dLng);
	}

	function circlesOverlap(point1, point2, value1, value2, zoom, minVal, maxVal) {
	    const [lng1, lat1] = point1.geometry ?
		[point1.geometry.coordinates[0], point1.geometry.coordinates[1]] :
		[point1.lng, point1.lat];
	    const [lng2, lat2] = point2.geometry ?
		[point2.geometry.coordinates[0], point2.geometry.coordinates[1]] :
		[point2.lng, point2.lat];

	    const scaleFactor1 = maxVal > minVal ? (value1 - minVal) / (maxVal - minVal) : 0.5;
	    const scaleFactor2 = maxVal > minVal ? (value2 - minVal) / (maxVal - minVal) : 0.5;

	    const radiusPixels1 = config.minRadius + (config.maxRadius - config.minRadius) * scaleFactor1;
	    const radiusPixels2 = config.minRadius + (config.maxRadius - config.minRadius) * scaleFactor2;

	    const radiusKm1 = pixelsToKm(radiusPixels1, (lat1 + lat2) / 2, zoom);
	    const radiusKm2 = pixelsToKm(radiusPixels2, (lat1 + lat2) / 2, zoom);

	    const distance = calculateDistance(point1, point2);

	    return distance < (radiusKm1 + radiusKm2);
	}

	function filterFeaturesByValue(features, category, property) {
	    return features.filter(f => {
		const val = getPropertyValue(f, category, property);
		return !isNaN(val) && val >= 1;
	    });
	}

	function reducePointsToEliminateOverlaps(features, category, property, maxDistance) {
	    if (features.length === 0) return [];

	    const zoom = map.getZoom();

	    const values = features.map(f => getPropertyValue(f, category, property));
	    const maxVal = Math.max(...values);
	    const minVal = Math.min(...values);

	    let points = features.map(f => ({
		feature: f,
		lat: f.geometry.coordinates[1],
		lng: f.geometry.coordinates[0],
		value: getPropertyValue(f, category, property) || 0,
		originalFeatures: [f]
	    }));

	    points.sort((a, b) => b.value - a.value);

	    let changed = true;
	    let iterations = 0;
	    const maxIterations = 50;

	    while (changed && iterations < maxIterations) {
		changed = false;
		iterations++;

		for (let i = 0; i < points.length; i++) {
		    if (!points[i]) continue;

		    for (let j = i + 1; j < points.length; j++) {
			if (!points[j]) continue;

			const distance = calculateDistance(points[i], points[j]);
			if (distance > maxDistance) continue;

			if (circlesOverlap(
			    points[i], points[j],
			    points[i].value, points[j].value,
			    zoom, minVal, maxVal
			)) {
			    const totalValue = points[i].value + points[j].value;

			    points[i].lat = (points[i].lat * points[i].value + points[j].lat * points[j].value) / totalValue;
			    points[i].lng = (points[i].lng * points[i].value + points[j].lng * points[j].value) / totalValue;
			    points[i].value = totalValue;

			    points[i].originalFeatures = points[i].originalFeatures.concat(points[j].originalFeatures);

			    points.splice(j, 1);
			    j--;
			    changed = true;
			}
		    }
		}
	    }

	    return points;
	}

	function updateMap() {
	    if (!originalFeatures.length) return;

	    const maxDistance = parseFloat(document.getElementById('distance-slider').value);

	    config.minRadius = parseInt(document.getElementById('min-radius').value);
	    config.maxRadius = parseInt(document.getElementById('max-radius').value);
	    config.fillOpacity = parseFloat(document.getElementById('fill-opacity').value) / 100;

	    const filteredFeatures = filterFeaturesByValue(originalFeatures, currentCategory, currentProperty);
	    const zeroValueCount = originalFeatures.length - filteredFeatures.length;

	    const statusDiv = document.getElementById('status-message');
	    if (filteredFeatures.length === 0) {
		statusDiv.className = 'warning-info';
		statusDiv.innerHTML = \`⚠️ No points with \${currentCategory}.\${currentProperty} >= 1 found. Try another property.\`;
	    } else {
		statusDiv.className = 'success-info';
		statusDiv.innerHTML = \`✅ Found \${filteredFeatures.length} points with \${currentCategory}.\${currentProperty} >= 1\`;
	    }

	    currentReducedPoints = reducePointsToEliminateOverlaps(
		filteredFeatures,
		currentCategory,
		currentProperty,
		maxDistance
	    );

	    if (currentLayer) {
		map.removeLayer(currentLayer);
	    }

	    const values = currentReducedPoints.map(p => p.value);
	    const maxVal = Math.max(...values, 1);
	    const minVal = Math.min(...values, 1);

	    const reducedGeoJson = {
		type: "FeatureCollection",
		features: currentReducedPoints.map(point => ({
		    type: "Feature",
		    geometry: {
			type: "Point",
			coordinates: [point.lng, point.lat]
		    },
		    properties: {
			merged_count: point.originalFeatures.length,
			total_value: point.value,
			avg_value: point.value / point.originalFeatures.length,
			original_properties: point.originalFeatures.map(f => f.properties)
		    }
		}))
	    };

	    currentLayer = L.geoJSON(reducedGeoJson, {
		pointToLayer: function(feature, latlng) {
		    const value = feature.properties.total_value;

		    let radius = config.minRadius;

		    if (maxVal > minVal) {
			const scaleFactor = (value - minVal) / (maxVal - minVal);
			radius = config.minRadius + (config.maxRadius - config.minRadius) * scaleFactor;
		    } else {
			radius = (config.minRadius + config.maxRadius) / 2;
		    }

		    return L.circleMarker(latlng, {
			radius: radius,
			fillColor: '#3388ff',
			color: '#000',
			weight: 0.5,
			opacity: 0.8,
			fillOpacity: config.fillOpacity
		    });
		},

		onEachFeature: function(feature, layer) {
		    const props = feature.properties;

		    let popupContent = \`
			<div style="max-width: 300px;">
			    <h4>Merged Point (\${props.merged_count} locations)</h4>
			    <table style="border-collapse: collapse; width: 100%;">
				<tr><th>Total \${currentCategory}.\${currentProperty}:</th><td><strong>\${props.total_value.toFixed(2)}</strong></td></tr>
				<tr><th>Average:</th><td>\${props.avg_value.toFixed(2)}</td></tr>
				<tr><th colspan="2" style="padding-top: 10px;">Original values:</th></tr>
		    \`;

		    props.original_properties.slice(0, 5).forEach((origProps, idx) => {
			let origValue = 'N/A';
			try {
			    const parsed = JSON.parse(origProps[currentCategory]);
			    origValue = parsed[currentProperty];
			} catch (e) {
			    origValue = 'Error';
			}

			popupContent += \`
			    <tr><td colspan="2" style="border-top: 1px solid #eee; padding: 5px 0;">
				Point \${idx + 1}: \${origValue}
			    </td></tr>
			\`;
		    });

		    if (props.original_properties.length > 5) {
			popupContent += \`<tr><td colspan="2">...and \${props.original_properties.length - 5} more</td></tr>\`;
		    }

		    popupContent += \`</table></div>\`;

		    layer.bindPopup(popupContent);
		}
	    }).addTo(map);

	    updateLegend(currentCategory, currentProperty, minVal, maxVal, currentReducedPoints.length);

	    const reductionPercent = filteredFeatures.length > 0 ?
		((1 - currentReducedPoints.length/filteredFeatures.length) * 100).toFixed(1) : 0;

	    document.getElementById('point-counts').innerHTML =
		\`Original: \${originalFeatures.length} | Filtered: \${filteredFeatures.length} | Current: \${currentReducedPoints.length}\`;
	    document.getElementById('reduction-percent').innerHTML =
		\`Reduction: \${reductionPercent}%\`;
	    document.getElementById('merge-distance').innerHTML =
		\`Merge distance: \${maxDistance} km\`;
	    document.getElementById('value-range').innerHTML =
		\`Value range: \${minVal.toFixed(2)} - \${maxVal.toFixed(2)}\`;
	    document.getElementById('zero-count').innerHTML =
		\`Zero values: \${zeroValueCount} points hidden\`;

	    if (currentReducedPoints.length > 0) {
		map.fitBounds(currentLayer.getBounds());
	    }
	}

	function updateLegend(category, property, minVal, maxVal, numPoints) {
	    const smallRadius = config.minRadius;
	    const mediumRadius = (config.minRadius + config.maxRadius) / 2;
	    const largeRadius = config.maxRadius;

	    let legendHtml = \`
		<div class="legend-item">
		    <strong>Category:</strong> \${category}
		</div>
		<div class="legend-item">
		    <strong>Property:</strong> \${property}
		</div>
		<div class="legend-item">
		    <span>Min value: \${minVal.toFixed(2)}</span>
		</div>
		<div class="legend-item">
		    <div class="legend-circle" style="width: \${smallRadius}px; height: \${smallRadius}px; opacity: \${config.fillOpacity}; border: 0.5px solid #000;"></div>
		    <span>Small (\${smallRadius}px)</span>
		</div>
		<div class="legend-item">
		    <div class="legend-circle" style="width: \${mediumRadius}px; height: \${mediumRadius}px; opacity: \${config.fillOpacity}; border: 0.5px solid #000;"></div>
		    <span>Medium (\${Math.round(mediumRadius)}px)</span>
		</div>
		<div class="legend-item">
		    <div class="legend-circle" style="width: \${largeRadius}px; height: \${largeRadius}px; opacity: \${config.fillOpacity}; border: 0.5px solid #000;"></div>
		    <span>Max: \${maxVal.toFixed(2)} (\${largeRadius}px)</span>
		</div>
		<div class="legend-item">
		    <span>Fill opacity: \${(config.fillOpacity * 100).toFixed(0)}%</span>
		</div>
		<div class="legend-item">
		    <span>Stroke: 0.5pt black</span>
		</div>
		<div style="margin-top: 10px;">
		    <strong>Visible points:</strong> \${numPoints}
		</div>
	    \`;

	    document.getElementById('legend-content').innerHTML = legendHtml;
	}

	// Load GeoJSON data
	fetch(GEOJSON_URL)
	    .then(response => {
		if (!response.ok) {
		    throw new Error(\`HTTP error! status: \${response.status}\`);
		}
		return response.json();
	    })
	    .then(data => {
		geoJsonData = data;
		originalFeatures = data.features;

		console.log('First feature:', originalFeatures[0]);
		console.log('Downloaders JSON:', JSON.parse(originalFeatures[0].properties.downloaders));

		if (DEFAULT_CATEGORY === 'uploaders') {
		    document.getElementById('category-downloaders').classList.remove('active');
		    document.getElementById('category-uploaders').classList.add('active');
		}

		document.getElementById('property-select').value = DEFAULT_PROPERTY;
		currentProperty = DEFAULT_PROPERTY;

		document.getElementById('min-radius').value = config.minRadius;
		document.getElementById('max-radius').value = config.maxRadius;
		document.getElementById('fill-opacity').value = config.fillOpacity * 100;

		map.whenReady(() => {
		    setTimeout(updateMap, 500);
		});
	    })
	    .catch(error => {
		console.error('Error loading GeoJSON:', error);
		document.body.innerHTML += \`<p style="color: red; position: absolute; top: 50px; left: 50px; background: white; padding: 10px; z-index: 2000; border-radius: 5px; box-shadow: 0 0 15px rgba(0,0,0,0.2);">❌ Error loading GeoJSON: \${error.message}<br>URL: \${GEOJSON_URL}</p>\`;
	    });

	// Event listeners
	document.getElementById('category-downloaders').addEventListener('click', function() {
	    this.classList.add('active');
	    document.getElementById('category-uploaders').classList.remove('active');
	    currentCategory = 'downloaders';
	    updateMap();
	});

	document.getElementById('category-uploaders').addEventListener('click', function() {
	    this.classList.add('active');
	    document.getElementById('category-downloaders').classList.remove('active');
	    currentCategory = 'uploaders';
	    updateMap();
	});

	document.getElementById('property-select').addEventListener('change', function(e) {
	    currentProperty = e.target.value;
	    updateMap();
	});

	document.getElementById('distance-slider').addEventListener('input', function(e) {
	    const value = e.target.value;
	    document.getElementById('distance-value').textContent = value + ' km';
	});

	document.getElementById('apply-reduction').addEventListener('click', function() {
	    updateMap();
	});

	document.getElementById('apply-style').addEventListener('click', function() {
	    updateMap();
	});

	document.getElementById('reset-view').addEventListener('click', function() {
	    map.setView(DEFAULT_MAP_CENTER, DEFAULT_MAP_ZOOM);
	});
    </script>
</body>
</html>`;
}
