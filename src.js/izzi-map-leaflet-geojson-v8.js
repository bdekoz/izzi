/**
 * DeepThink 20260329 
 * Version 8.0 - Leaflet map visualization for Andor cumulative P2P activity data
 * Supports native GeoJSON format with clustering and visualization controls
 * 
 * @param {string} geojsonUrl - URL to the GeoJSON data file
 * @param {string} elementId - ID of the DOM element to render the map in (default: 'map')
 */
function leaflet_map_geojson(geojsonUrl, elementId = 'map') {
    const VERSION = '8.0';
    
    // Check if Leaflet is loaded
    if (typeof L === 'undefined') {
        console.error('Leaflet library not loaded. Please include Leaflet CSS and JS.');
        return;
    }
    
    // Check if Leaflet.markercluster is loaded
    const hasCluster = typeof L.markerClusterGroup !== 'undefined';
    if (!hasCluster) {
        console.warn('Leaflet.markercluster not loaded. Clustering disabled.');
    }
    
    // Create map container if it doesn't exist
    let mapContainer = document.getElementById(elementId);
    if (!mapContainer) {
        mapContainer = document.createElement('div');
        mapContainer.id = elementId;
        mapContainer.style.height = '100vh';
        mapContainer.style.width = '100%';
        document.body.appendChild(mapContainer);
    }
    
    // Clear any existing content
    mapContainer.innerHTML = '';
    
    // Add loading indicator
    const loadingDiv = document.createElement('div');
    loadingDiv.id = 'loading-indicator';
    loadingDiv.style.position = 'absolute';
    loadingDiv.style.top = '50%';
    loadingDiv.style.left = '50%';
    loadingDiv.style.transform = 'translate(-50%, -50%)';
    loadingDiv.style.background = 'rgba(0,0,0,0.8)';
    loadingDiv.style.color = 'white';
    loadingDiv.style.padding = '15px 30px';
    loadingDiv.style.borderRadius = '8px';
    loadingDiv.style.zIndex = '2000';
    loadingDiv.style.fontSize = '16px';
    loadingDiv.innerHTML = 'Loading data...';
    mapContainer.appendChild(loadingDiv);
    
    // Initialize map
    const map = L.map(elementId).setView([20, 0], 2);
    
    // Add base tile layer
    L.tileLayer('https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png', {
        attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> &copy; CartoDB',
        subdomains: 'abcd',
        maxZoom: 19
    }).addTo(map);
    
    // Create control panel
    const controlPanel = document.createElement('div');
    controlPanel.className = 'leaflet-control leaflet-bar';
    controlPanel.style.position = 'absolute';
    controlPanel.style.top = '10px';
    controlPanel.style.right = '10px';
    controlPanel.style.background = 'white';
    controlPanel.style.padding = '12px 15px';
    controlPanel.style.borderRadius = '8px';
    controlPanel.style.boxShadow = '0 2px 8px rgba(0,0,0,0.2)';
    controlPanel.style.zIndex = '1000';
    controlPanel.style.minWidth = '220px';
    controlPanel.style.fontSize = '12px';
    controlPanel.style.pointerEvents = 'auto';
    controlPanel.innerHTML = `
        <h3 style="margin: 0 0 10px 0; font-size: 14px;">🎛️ Visualization Controls</h3>
        <div style="margin-bottom: 10px;">
            <label style="font-weight: bold;">Visualize by:</label>
            <select id="viz-select" style="width: 100%; margin-top: 5px; padding: 4px; border-radius: 4px; border: 1px solid #ccc;">
                <option value="total">Total Peers (Downloaders + Uploaders)</option>
                <option value="downloaders">Downloaders Only</option>
                <option value="uploaders">Uploaders Only</option>
                <option value="ratio">Downloader/Uploader Ratio</option>
            </select>
        </div>
        <div style="margin-bottom: 10px;">
            <label style="font-weight: bold;">Max Merge Distance (km):</label>
            <input type="range" id="merge-distance" min="0" max="50" step="5" value="20" style="width: 100%; margin-top: 5px;">
            <span id="distance-value" style="font-size: 11px; color: #666;">20 km</span>
        </div>
        <div style="margin-bottom: 10px;">
            <label style="font-weight: bold;">Circle Style:</label>
            <select id="circle-style" style="width: 100%; margin-top: 5px; padding: 4px; border-radius: 4px; border: 1px solid #ccc;">
                <option value="size">Size = Value, Color = Fixed</option>
                <option value="color">Color = Value, Size = Fixed</option>
                <option value="both">Both Size and Color = Value</option>
            </select>
        </div>
        <div style="margin-bottom: 5px;">
            <label style="font-weight: bold;">Show Labels:</label>
            <input type="checkbox" id="show-labels" style="margin-left: 8px;">
        </div>
        <hr style="margin: 10px 0;">
        <div style="font-size: 11px; color: #666;">
            <div>📍 Total locations: <span id="total-locations" style="font-weight: bold;">0</span></div>
            <div>📥 Total downloaders: <span id="total-downloaders" style="font-weight: bold;">0</span></div>
            <div>📤 Total uploaders: <span id="total-uploaders" style="font-weight: bold;">0</span></div>
            <div>🔵 Version: ${VERSION}</div>
        </div>
    `;
    mapContainer.appendChild(controlPanel);
    
    // Add version badge
    const versionBadge = document.createElement('div');
    versionBadge.style.position = 'absolute';
    versionBadge.style.bottom = '10px';
    versionBadge.style.left = '10px';
    versionBadge.style.background = 'rgba(0,0,0,0.6)';
    versionBadge.style.color = 'white';
    versionBadge.style.padding = '4px 8px';
    versionBadge.style.borderRadius = '4px';
    versionBadge.style.fontSize = '11px';
    versionBadge.style.fontFamily = 'monospace';
    versionBadge.style.zIndex = '1000';
    versionBadge.style.pointerEvents = 'none';
    versionBadge.innerHTML = `v${VERSION} | Native GeoJSON | Clustering Enabled`;
    mapContainer.appendChild(versionBadge);
    
    // Store data and markers
    let geoJsonData = null;
    let currentMarkers = [];
    let currentClusterGroup = null;
    let labelsLayer = null;
    
    // Helper: Format numbers with commas
    function formatNumber(num) {
        return num.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
    }
    
    // Helper: Get value based on visualization type
    function getValue(feature, vizType) {
        const downloaders = feature.properties?.downloaders?.size || 0;
        const uploaders = feature.properties?.uploaders?.size || 0;
        
        switch(vizType) {
            case 'downloaders':
                return downloaders;
            case 'uploaders':
                return uploaders;
            case 'ratio':
                if (uploaders === 0) return downloaders > 0 ? 100 : 0;
                return downloaders / uploaders;
            default:
                return downloaders + uploaders;
        }
    }
    
    // Helper: Get color based on value and visualization type
    function getColor(value, vizType) {
        if (vizType === 'ratio') {
            // Color scale for ratio
            if (value > 3) return '#2c7bb6';
            if (value > 1.5) return '#abd9e9';
            if (value >= 0.67) return '#ffffbf';
            if (value >= 0.33) return '#fdae61';
            return '#d7191c';
        } else {
            // Color scale for counts (log scale for better distribution)
            const logVal = Math.log10(value + 1);
            const maxLog = Math.log10(100000 + 1);
            const intensity = Math.min(0.8, logVal / maxLog);
            
            if (vizType === 'uploaders') {
                return `rgba(220, 20, 60, ${0.3 + intensity * 0.5})`;
            } else {
                return `rgba(44, 123, 182, ${0.3 + intensity * 0.5})`;
            }
        }
    }
    
    // Helper: Get radius based on value and circle style
    function getRadius(value, vizType, circleStyle, valueType) {
        const baseRadius = 8;
        const maxRadius = 35;
        
        if (circleStyle === 'color' || (circleStyle === 'both' && valueType !== 'size')) {
            return baseRadius;
        }
        
        // Use log scale for radius
        const logVal = Math.log10(value + 1);
        const maxLog = Math.log10(100000 + 1);
        return baseRadius + (logVal / maxLog) * (maxRadius - baseRadius);
    }
    
    // Helper: Get circle color
    function getCircleColor(value, vizType, circleStyle, valueType) {
        if (circleStyle === 'size' || (circleStyle === 'both' && valueType === 'size')) {
            // Fixed blue for size-only mode
            return '#2c7bb6';
        }
        return getColor(value, vizType);
    }
    
    // Create circle marker for a feature
    function createCircleMarker(feature, vizType, circleStyle, showLabel = false) {
        const coords = feature.geometry.coordinates;
        if (!coords || coords.length < 2) return null;
        
        const downloaders = feature.properties?.downloaders?.size || 0;
        const uploaders = feature.properties?.uploaders?.size || 0;
        const totalPeers = downloaders + uploaders;
        
        if (totalPeers === 0) return null;
        
        const value = getValue(feature, vizType);
        const radius = getRadius(value, vizType, circleStyle, 'size');
        const color = getCircleColor(value, vizType, circleStyle, 'color');
        
        const popupContent = `
            <div style="min-width: 200px;">
                <h3>${feature.properties?.city || 'Unknown'}, ${feature.properties?.country_code || 'Unknown'}</h3>
                <p><strong>Downloaders:</strong> ${formatNumber(downloaders)}</p>
                <p><strong>Uploaders:</strong> ${formatNumber(uploaders)}</p>
                <p><strong>Total Peers:</strong> ${formatNumber(totalPeers)}</p>
                <p><strong>Ratio (D/U):</strong> ${uploaders === 0 ? '∞' : (downloaders / uploaders).toFixed(2)}</p>
                <hr>
                <p><strong>Downloader Breakdown:</strong></p>
                <ul style="margin: 5px 0; font-size: 12px;">
                    <li>Mobile: ${formatNumber(feature.properties?.downloaders?.mobile || 0)}</li>
                    <li>Satellite: ${formatNumber(feature.properties?.downloaders?.satellite || 0)}</li>
                    <li>VPN: ${formatNumber(feature.properties?.downloaders?.vpn || 0)}</li>
                    <li>Hosting: ${formatNumber(feature.properties?.downloaders?.hosting || 0)}</li>
                    <li>Service: ${formatNumber(feature.properties?.downloaders?.service || 0)}</li>
                </ul>
                <p><strong>Uploader Breakdown:</strong></p>
                <ul style="margin: 5px 0; font-size: 12px;">
                    <li>Mobile: ${formatNumber(feature.properties?.uploaders?.mobile || 0)}</li>
                    <li>Satellite: ${formatNumber(feature.properties?.uploaders?.satellite || 0)}</li>
                    <li>VPN: ${formatNumber(feature.properties?.uploaders?.vpn || 0)}</li>
                    <li>Hosting: ${formatNumber(feature.properties?.uploaders?.hosting || 0)}</li>
                    <li>Service: ${formatNumber(feature.properties?.uploaders?.service || 0)}</li>
                </ul>
            </div>
        `;
        
        const circle = L.circleMarker([coords[1], coords[0]], {
            radius: radius,
            fillColor: color,
            color: '#333',
            weight: 1,
            opacity: 0.6,
            fillOpacity: 0.7
        });
        
        circle.bindPopup(popupContent);
        
        // Add label if enabled
        if (showLabel) {
            const label = L.marker([coords[1], coords[0]], {
                icon: L.divIcon({
                    className: 'circle-label',
                    html: `<div style="background: rgba(0,0,0,0.7); color: white; padding: 2px 5px; border-radius: 3px; font-size: 10px; white-space: nowrap;">${feature.properties?.city || ''}</div>`,
                    iconSize: [60, 20],
                    popupAnchor: [0, -10]
                })
            });
            circle.label = label;
        }
        
        return circle;
    }
    
    // Update visualization
    function updateVisualization() {
        const vizType = document.getElementById('viz-select').value;
        const mergeDistance = parseInt(document.getElementById('merge-distance').value);
        const circleStyle = document.getElementById('circle-style').value;
        const showLabels = document.getElementById('show-labels').checked;
        
        document.getElementById('distance-value').textContent = `${mergeDistance} km`;
        
        // Clear existing markers
        if (currentClusterGroup) {
            map.removeLayer(currentClusterGroup);
        }
        if (labelsLayer) {
            map.removeLayer(labelsLayer);
        }
        
        if (!geoJsonData) return;
        
        // Create markers
        const markers = [];
        const labelMarkers = [];
        
        geoJsonData.features.forEach(feature => {
            const marker = createCircleMarker(feature, vizType, circleStyle, showLabels);
            if (marker) {
                markers.push(marker);
                if (showLabels && marker.label) {
                    labelMarkers.push(marker.label);
                }
            }
        });
        
        // Apply clustering if available
        if (hasCluster && markers.length > 0) {
            // Convert km to pixels approximation (1km ~ 0.01 degrees at equator, rough estimate)
            const mergePixels = mergeDistance * 10;
            
            currentClusterGroup = L.markerClusterGroup({
                maxClusterRadius: mergePixels,
                disableClusteringAtZoom: 18,
                spiderfyOnMaxZoom: true,
                showCoverageOnHover: true,
                zoomToBoundsOnClick: true,
                iconCreateFunction: function(cluster) {
                    const childCount = cluster.getChildCount();
                    let size = 'small';
                    if (childCount > 100) size = 'large';
                    else if (childCount > 10) size = 'medium';
                    
                    return L.divIcon({
                        html: `<div style="background-color: #ff6b35; border-radius: 50%; width: ${size === 'large' ? 40 : size === 'medium' ? 30 : 20}px; height: ${size === 'large' ? 40 : size === 'medium' ? 30 : 20}px; display: flex; align-items: center; justify-content: center; color: white; font-weight: bold; border: 2px solid white;">${childCount}</div>`,
                        className: 'marker-cluster',
                        iconSize: size === 'large' ? [40, 40] : size === 'medium' ? [30, 30] : [20, 20]
                    });
                }
            });
            
            currentClusterGroup.addLayers(markers);
            map.addLayer(currentClusterGroup);
            
            if (showLabels && labelMarkers.length > 0) {
                labelsLayer = L.layerGroup(labelMarkers);
                map.addLayer(labelsLayer);
            }
        } else {
            // No clustering, add directly
            markers.forEach(marker => map.addLayer(marker));
            if (showLabels && labelMarkers.length > 0) {
                labelsLayer = L.layerGroup(labelMarkers);
                map.addLayer(labelsLayer);
            }
            currentClusterGroup = null;
        }
        
        currentMarkers = markers;
        
        // Update legend
        updateLegend(vizType, circleStyle);
    }
    
    // Update legend based on current visualization
    function updateLegend(vizType, circleStyle) {
        // Remove existing legend if present
        const oldLegend = document.querySelector('.viz-legend');
        if (oldLegend) oldLegend.remove();
        
        const legend = document.createElement('div');
        legend.className = 'viz-legend leaflet-control leaflet-bar';
        legend.style.position = 'absolute';
        legend.style.bottom = '30px';
        legend.style.right = '10px';
        legend.style.background = 'white';
        legend.style.padding = '10px 15px';
        legend.style.borderRadius = '8px';
        legend.style.boxShadow = '0 2px 8px rgba(0,0,0,0.2)';
        legend.style.zIndex = '1000';
        legend.style.fontSize = '12px';
        legend.style.lineHeight = '1.4';
        legend.style.pointerEvents = 'none';
        
        if (vizType === 'ratio') {
            legend.innerHTML = `
                <h4 style="margin: 0 0 8px 0;">📊 Downloader/Uploader Ratio</h4>
                <div><span style="background: #2c7bb6; width: 20px; height: 20px; display: inline-block; margin-right: 8px;"></span> &gt;3:1 (High Downloaders)</div>
                <div><span style="background: #abd9e9; width: 20px; height: 20px; display: inline-block; margin-right: 8px;"></span> 1.5-3:1</div>
                <div><span style="background: #ffffbf; width: 20px; height: 20px; display: inline-block; margin-right: 8px;"></span> 0.67-1.5:1 (Balanced)</div>
                <div><span style="background: #fdae61; width: 20px; height: 20px; display: inline-block; margin-right: 8px;"></span> 0.33-0.67:1</div>
                <div><span style="background: #d7191c; width: 20px; height: 20px; display: inline-block; margin-right: 8px;"></span> &lt;0.33:1 (High Uploaders)</div>
            `;
        } else {
            const metric = vizType === 'downloaders' ? 'Downloaders' : (vizType === 'uploaders' ? 'Uploaders' : 'Total Peers');
            const color = vizType === 'uploaders' ? '#dc143c' : '#2c7bb6';
            
            if (circleStyle === 'size' || circleStyle === 'both') {
                legend.innerHTML = `
                    <h4 style="margin: 0 0 8px 0;">📊 ${metric}</h4>
                    <div><span style="background: ${color}; width: 20px; height: 20px; display: inline-block; margin-right: 8px; border-radius: 50%;"></span> Circle Size = Value</div>
                    <div style="margin-top: 8px; font-size: 10px; color: #666;">● Larger circle = more ${metric.toLowerCase()}</div>
                `;
            } else {
                legend.innerHTML = `
                    <h4 style="margin: 0 0 8px 0;">📊 ${metric}</h4>
                    <div><span style="background: ${color}40; width: 20px; height: 20px; display: inline-block; margin-right: 8px;"></span> Low</div>
                    <div><span style="background: ${color}80; width: 20px; height: 20px; display: inline-block; margin-right: 8px;"></span> Medium</div>
                    <div><span style="background: ${color}; width: 20px; height: 20px; display: inline-block; margin-right: 8px;"></span> High</div>
                    <div style="margin-top: 8px; font-size: 10px; color: #666;">Darker color = more ${metric.toLowerCase()}</div>
                `;
            }
        }
        
        mapContainer.appendChild(legend);
    }
    
    // Load and process GeoJSON
    async function loadData() {
        try {
            const response = await fetch(geojsonUrl);
            
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            
            // Parse directly as JSON - native GeoJSON format
            geoJsonData = await response.json();
            
            // Remove loading indicator
            if (loadingDiv) loadingDiv.style.display = 'none';
            
            // Validate data structure
            if (!geoJsonData.features || !Array.isArray(geoJsonData.features)) {
                throw new Error('Invalid GeoJSON structure: missing features array');
            }
            
            // Log data version if available
            if (geoJsonData.data_version) {
                console.log(`GeoJSON data version: ${geoJsonData.data_version}`);
            }
            
            // Calculate totals
            let totalDownloaders = 0;
            let totalUploaders = 0;
            
            geoJsonData.features.forEach(feature => {
                const props = feature.properties;
                if (props && props.downloaders && props.downloaders.size) {
                    totalDownloaders += props.downloaders.size;
                }
                if (props && props.uploaders && props.uploaders.size) {
                    totalUploaders += props.uploaders.size;
                }
            });
            
            // Update info panel
            const totalLocationsSpan = document.getElementById('total-locations');
            const totalDownloadersSpan = document.getElementById('total-downloaders');
            const totalUploadersSpan = document.getElementById('total-uploaders');
            
            if (totalLocationsSpan) totalLocationsSpan.textContent = formatNumber(geoJsonData.features.length);
            if (totalDownloadersSpan) totalDownloadersSpan.textContent = formatNumber(totalDownloaders);
            if (totalUploadersSpan) totalUploadersSpan.textContent = formatNumber(totalUploaders);
            
            // Add event listeners for controls
            document.getElementById('viz-select').addEventListener('change', updateVisualization);
            document.getElementById('merge-distance').addEventListener('input', updateVisualization);
            document.getElementById('circle-style').addEventListener('change', updateVisualization);
            document.getElementById('show-labels').addEventListener('change', updateVisualization);
            
            // Initial visualization
            updateVisualization();
            
            console.log(`v${VERSION}: Loaded ${geoJsonData.features.length} locations`);
            
        } catch (error) {
            console.error(`v${VERSION} Error loading GeoJSON:`, error);
            if (loadingDiv) {
                loadingDiv.innerHTML = `Error loading data: ${error.message}`;
                loadingDiv.style.background = 'rgba(200,0,0,0.9)';
            }
        }
    }
    
    // Start loading data
    loadData();
    
    // Return map instance for potential additional customization
    return map;
}
