// 20260801
// baseline-v7.5 - Leaflet GeoJSON map on cartofreako Cahill-Keyes cartography.
// Based on v7.4, retaining gzip input, week/cumulative title generation,
// point reduction, category/property controls, and the new-window wrapper.

/*
 * Native JavaScript port of cartofreako's C++20 forward projection in
 * src/cart0freak0-cahill-keyes.h. The geometry derives from the
 * MegamapMaker-prep9.pl work of Mary Jo Graça and Gene Keyes.
 *
 * The upstream algorithm is distributed for non-commercial use with
 * attribution. Commercial users should contact Gene Keyes.
 */
class CartofreakoCahillKeyesProjection {
    constructor(width, height) {
        const expectedWidth = 2 * height;
        const tolerance = 16 * Number.EPSILON * Math.max(width, expectedWidth);
        if (!Number.isFinite(width) || !Number.isFinite(height)
            || width <= 0 || height <= 0
            || Math.abs(width - expectedWidth) > tolerance) {
            throw new RangeError(
                'Cahill-Keyes frame must have finite, positive dimensions '
                + 'with a 2:1 width-to-height ratio'
            );
        }

        this.width = width;
        this.height = height;
        this.radians = Math.PI / 180;
        this.lengthMG = height / 2;
        this.scale = this.lengthMG / 10000;
        this.lengthMA = 940 * this.scale;
        this.latitudeDegree100 = 100 * this.scale;
        this.latitudeDegree104 = 104 * this.scale;
        this.sin60 = Math.sqrt(3) / 2;
        this.cos60 = 0.5;
        this.yTranslate = this.lengthMG * this.sin60;

        this.pointM = [0, 0];
        this.pointG = [this.lengthMG, 0];
        this.pointA = [this.lengthMA, 0];
        this.pointB = [0, 0];
        this.pointC = [0, 0];
        this.pointD = [0, 0];
        this.pointE = [0, 0];
        this.pointF = [0, 0];
        this.pointT = [0, 0];

        this.lengthAB = 0;
        this.lengthGF = 0;
        this.deltaMEquator = 0;
        this.lengthAP73 = 1760 * this.scale;
        this.lengthAP75 = 1560 * this.scale;
        this.radius = 0;

        this.calculatePreliminaries();
    }

    distance(first, second) {
        return Math.hypot(first[0] - second[0], first[1] - second[1]);
    }

    interpolate(length, total, start, end) {
        if (total === 0) {
            throw new RangeError(
                'Cahill-Keyes interpolation over a zero-length segment'
            );
        }
        const ratio = length / total;
        return [
            start[0] + (end[0] - start[0]) * ratio,
            start[1] + (end[1] - start[1]) * ratio
        ];
    }

    lineIntersection(first, firstSlope, second, secondSlope) {
        const m1 = Math.tan(firstSlope * this.radians);
        const m2 = Math.tan(secondSlope * this.radians);
        const x = (
            m1 * first[0] - m2 * second[0] - first[1] + second[1]
        ) / (m1 - m2);
        return [x, m1 * (x - first[0]) + first[1]];
    }

    intersectCircleLine(center, radius, first, second) {
        const dx = second[0] - first[0];
        const dy = second[1] - first[1];
        const a = dx * dx + dy * dy;
        if (a === 0) return {intersects: false, point: [0, 0]};

        const b = 2 * (
            dx * (first[0] - center[0])
            + dy * (first[1] - center[1])
        );
        const c = center[0] * center[0] + center[1] * center[1]
            + first[0] * first[0] + first[1] * first[1]
            - 2 * (center[0] * first[0] + center[1] * first[1])
            - radius * radius;
        const determinant = b * b - 4 * a * c;
        if (determinant < 0) {
            return {intersects: false, point: [0, 0]};
        }

        const root = Math.sqrt(Math.max(0, determinant));
        const factors = [
            (-b + root) / (2 * a),
            (-b - root) / (2 * a)
        ];
        for (const factor of factors) {
            if (factor >= 0 && factor <= 1) {
                return {
                    intersects: true,
                    point: [first[0] + factor * dx, first[1] + factor * dy]
                };
            }
        }
        return {intersects: false, point: [0, 0]};
    }

    rotate(point, angle) {
        if (angle === -60) {
            return [
                point[0] * this.cos60 + point[1] * this.sin60,
                -point[0] * this.sin60 + point[1] * this.cos60
            ];
        }
        if (angle === -120) {
            return [
                -point[0] * this.cos60 + point[1] * this.sin60,
                -point[0] * this.sin60 - point[1] * this.cos60
            ];
        }
        throw new RangeError('Unsupported Cahill-Keyes octant rotation');
    }

    equator(meridian) {
        let length = this.deltaMEquator * meridian;
        if (length <= this.lengthGF) {
            return [this.pointG[0], length];
        }
        length -= this.lengthGF;
        return this.interpolate(
            length, this.lengthAB, this.pointF, this.pointE
        );
    }

    jointT(meridian) {
        return this.lineIntersection(
            this.pointM, 2 * meridian / 3,
            this.equator(meridian), meridian / 3
        );
    }

    jointF(meridian) {
        if (meridian === 0) {
            return [this.pointA[0] + this.lengthAB, 0];
        }
        return this.lineIntersection(
            this.pointA, meridian, this.pointM, 2 * meridian / 3
        );
    }

    torridLength(meridian) {
        return this.distance(this.equator(meridian), this.jointT(meridian));
    }

    middleLength(meridian) {
        return this.distance(this.jointT(meridian), this.jointF(meridian));
    }

    parallel73(meridian) {
        const jointF = this.jointF(meridian);
        let point;
        let length;
        if (meridian <= 30) {
            point = [
                this.pointA[0]
                    + this.lengthAP73 * Math.cos(meridian * this.radians),
                this.pointA[1]
                    + this.lengthAP73 * Math.sin(meridian * this.radians)
            ];
            length = this.distance(jointF, point);
        } else {
            point = this.lineIntersection(
                this.pointT, -60, jointF, meridian
            );
            length = this.distance(jointF, point);
            if (meridian > 44) {
                const middle = this.lineIntersection(
                    this.pointT, -60, jointF, 2 * meridian / 3
                );
                if (middle[0] > point[0]) {
                    point = middle;
                    length = -this.distance(jointF, point);
                }
            }
        }
        return {point, length};
    }

    parallel75(meridian) {
        return [
            this.pointA[0]
                + this.lengthAP75 * Math.cos(meridian * this.radians),
            this.pointA[1]
                + this.lengthAP75 * Math.sin(meridian * this.radians)
        ];
    }

    longitudeLatitudeToMeridian(longitude, latitude) {
        let octant = Math.trunc((longitude + 200) / 90) + 1;
        let meridian = longitude + 200 - 90 * (octant - 1) - 45;
        const sign = meridian < 0 ? -1 : 1;
        meridian = Math.abs(meridian);
        if (octant === 5) octant = 1;
        if (latitude < 0) {
            const southernOctants = [0, 6, 7, 8, 5];
            octant = southernOctants[octant];
        }
        return {
            meridian,
            parallel: Math.abs(latitude),
            sign,
            octant
        };
    }

    zoneH(meridian, parallel) {
        const p75 = this.parallel75(45);
        const p73 = this.parallel73(meridian).point;
        const lf = this.distance(this.pointT, this.pointB);
        const lf75 = this.distance(this.pointB, p75);
        let length = (75 - parallel) * (lf75 + lf) / 2;
        if (length <= lf75) {
            return this.interpolate(length, lf75, p75, this.pointB);
        }
        length -= lf75;
        return this.interpolate(length, lf, this.pointB, p73);
    }

    zoneI(meridian, parallel) {
        const p73 = this.parallel73(meridian);
        const lt = this.torridLength(meridian);
        const lm = this.middleLength(meridian);
        let length = parallel * (lt + lm + p73.length) / 73;
        if (length <= lt) {
            return this.interpolate(
                length, lt, this.equator(meridian), this.jointT(meridian)
            );
        }
        if (length <= lt + lm) {
            return this.interpolate(
                length - lt, lm, this.jointT(meridian),
                this.jointF(meridian)
            );
        }
        return this.interpolate(
            length - lt - lm, p73.length, this.jointF(meridian), p73.point
        );
    }

    zoneJ(meridian, parallel) {
        const p75 = this.parallel75(meridian);
        const p73 = this.parallel73(meridian);
        const lf75 = this.distance(this.jointF(meridian), p75);
        let length = (75 - parallel) * (lf75 - p73.length) / 2;
        if (length <= lf75) {
            return this.interpolate(
                length, lf75, p75, this.jointF(meridian)
            );
        }
        length -= lf75;
        return this.interpolate(
            length, -p73.length, this.jointF(meridian), p73.point
        );
    }

    zoneK(meridian, parallel, length15) {
        let length = parallel * length15 / 15;
        const lt = this.torridLength(meridian);
        if (length <= lt) {
            return this.interpolate(
                length, lt, this.equator(meridian), this.jointT(meridian)
            );
        }
        return this.interpolate(
            length - lt, this.middleLength(meridian),
            this.jointT(meridian), this.jointF(meridian)
        );
    }

    zoneL(meridian, parallel, length15) {
        const p73 = this.parallel73(meridian);
        const lt = this.torridLength(meridian);
        const lm = this.middleLength(meridian);
        let length = length15
            + (parallel - 15)
            * ((lt + lm + p73.length) - length15) / 58;
        if (length <= lt) {
            return this.interpolate(
                length, lt, this.equator(meridian), this.jointT(meridian)
            );
        }
        if (length <= lt + lm) {
            return this.interpolate(
                length - lt, lm, this.jointT(meridian),
                this.jointF(meridian)
            );
        }
        return this.interpolate(
            length - lt - lm, p73.length, this.jointF(meridian), p73.point
        );
    }

    meridianParallelToXy(meridian, parallel) {
        if (meridian === 0) {
            return parallel >= 75
                ? [
                    this.pointA[0]
                        + (90 - parallel) * this.latitudeDegree104,
                    0
                ]
                : [
                    this.pointG[0]
                        - parallel * this.latitudeDegree100,
                    0
                ];
        }

        if (parallel >= 75) {
            const length = this.latitudeDegree104 * (90 - parallel);
            return [
                this.pointA[0]
                    + length * Math.cos(meridian * this.radians),
                this.pointA[1]
                    + length * Math.sin(meridian * this.radians)
            ];
        }

        if (parallel === 0) return this.equator(meridian);

        if (parallel >= 73 && meridian <= 30) {
            const length = this.lengthAP75
                + (75 - parallel) * this.latitudeDegree100;
            return [
                this.pointA[0]
                    + length * Math.cos(meridian * this.radians),
                this.pointA[1]
                    + length * Math.sin(meridian * this.radians)
            ];
        }

        if (meridian === 45) {
            if (parallel <= 15) {
                return this.interpolate(
                    parallel, 15, this.pointE, this.pointD
                );
            }
            if (parallel <= 73) {
                return this.interpolate(
                    parallel - 15, 58, this.pointD, this.pointT
                );
            }
            return this.zoneH(meridian, parallel);
        }

        if (meridian <= 29) return this.zoneI(meridian, parallel);
        if (parallel >= 73) return this.zoneJ(meridian, parallel);

        const jointT = this.jointT(meridian);
        const jointF = this.jointF(meridian);
        let p15 = this.intersectCircleLine(
            this.pointC, this.radius, jointT, jointF
        );
        const lt = this.torridLength(meridian);
        let length15;
        if (p15.intersects) {
            length15 = lt + this.distance(jointT, p15.point);
        } else {
            p15 = this.intersectCircleLine(
                this.pointC, this.radius, this.equator(meridian), jointT
            );
            if (!p15.intersects) {
                throw new RangeError(
                    'Cahill-Keyes parallel 15 misses its meridian'
                );
            }
            length15 = lt - this.distance(jointT, p15.point);
        }
        return parallel <= 15
            ? this.zoneK(meridian, parallel, length15)
            : this.zoneL(meridian, parallel, length15);
    }

    halfOctantToMegamap(point, octant) {
        let result;
        if (octant === 1) {
            result = this.rotate(point, -120);
            result[0] -= this.lengthMG;
        } else if (octant === 2) {
            result = this.rotate(point, -60);
            result[0] -= this.lengthMG;
        } else if (octant === 3) {
            result = this.rotate(point, -120);
            result[0] += this.lengthMG;
        } else if (octant === 4) {
            result = this.rotate(point, -60);
            result[0] += this.lengthMG;
        } else if (octant === 5) {
            point = [2 * this.lengthMG - point[0], point[1]];
            result = this.rotate(point, -60);
            result[0] += this.lengthMG;
        } else if (octant === 6) {
            point = [2 * this.lengthMG - point[0], point[1]];
            result = this.rotate(point, -120);
            result[0] -= this.lengthMG;
        } else if (octant === 7) {
            point = [2 * this.lengthMG - point[0], point[1]];
            result = this.rotate(point, -60);
            result[0] -= this.lengthMG;
        } else if (octant === 8) {
            point = [2 * this.lengthMG - point[0], point[1]];
            result = this.rotate(point, -120);
            result[0] += this.lengthMG;
        } else {
            throw new RangeError('Invalid Cahill-Keyes octant');
        }
        result[1] += this.yTranslate;
        return result;
    }

    calculatePreliminaries() {
        const pointN = [
            this.lengthMG,
            this.lengthMG * Math.tan(30 * this.radians)
        ];
        this.pointB = this.lineIntersection(
            this.pointM, 30, this.pointA, 45
        );
        this.lengthAB = this.distance(this.pointA, this.pointB);
        const lengthMB = this.distance(this.pointM, this.pointB);
        const lengthMN = this.distance(this.pointM, pointN);
        this.pointD = this.interpolate(
            lengthMB, lengthMN, pointN, this.pointM
        );
        this.pointF = [this.lengthMG, pointN[1] - lengthMB];
        this.pointE = [
            pointN[0] - this.lengthMA * Math.sin(30 * this.radians),
            pointN[1] - this.lengthMA * Math.cos(30 * this.radians)
        ];
        this.lengthGF = this.distance(this.pointG, this.pointF);
        this.deltaMEquator = (this.lengthGF + this.lengthAB) / 45;

        const pointU = [
            this.pointA[0]
                + this.lengthAP73 * Math.cos(30 * this.radians),
            this.pointA[1]
                + this.lengthAP73 * Math.sin(30 * this.radians)
        ];
        this.pointT = this.lineIntersection(pointU, -60, this.pointB, 30);

        const meridian = 29;
        const parallel = 15;
        const p73 = this.parallel73(meridian);
        const lt = this.torridLength(meridian);
        const lm = this.middleLength(meridian);
        let length = parallel * (lt + lm + p73.length) / 73 - lt;
        const pointV = this.interpolate(
            length, lm, this.jointT(meridian), this.jointF(meridian)
        );
        const rootThree = Math.sqrt(3);
        this.pointC[1] = (
            pointV[0] * pointV[0] + pointV[1] * pointV[1]
            - this.pointD[0] * this.pointD[0]
            - this.pointD[1] * this.pointD[1]
        ) / (
            2 * (
                rootThree * pointV[0] + pointV[1]
                - rootThree * this.pointD[0] - this.pointD[1]
            )
        );
        this.pointC[0] = rootThree * this.pointC[1];
        this.radius = this.distance(this.pointC, this.pointD);
    }

    /*
     * Transform latitude/longitude degrees through the same public calling
     * convention and one-degree raster registration as cartofreako ckproj.
     */
    project(latitude, longitude) {
        if (!Number.isFinite(latitude) || latitude < -90 || latitude > 90) {
            throw new RangeError(
                'Cahill-Keyes latitude must be within [-90, 90]'
            );
        }
        if (!Number.isFinite(longitude)
            || longitude < -180 || longitude > 180) {
            throw new RangeError(
                'Cahill-Keyes longitude must be within [-180, 180]'
            );
        }

        let adjustedLongitude = longitude + 1;
        if (adjustedLongitude > 180) adjustedLongitude -= 360;
        const location = this.longitudeLatitudeToMeridian(
            adjustedLongitude, latitude
        );
        const halfOctant = this.meridianParallelToXy(
            location.meridian, location.parallel
        );
        halfOctant[1] *= location.sign;
        const megamap = this.halfOctantToMegamap(
            halfOctant, location.octant
        );
        return {
            x: this.width / 2 + megamap[0],
            y: this.height / 2 - megamap[1]
        };
    }
}

function leaflet_map_geojson(geojsonUrl) {
    // Extract title from URL – handles -cumulative, -week-*, extensions, query strings
    function getTitleFromUrl(url) {
        let cleanUrl = url.split('?')[0];                 // strip query params
        let fileName = cleanUrl.split('/').pop();         // get base filename
        fileName = fileName.replace(
            /(?:\.(?:geojson|json|zip|gz|gzip|bz2))+$/i, ''
        ); // strip stacked data/compression extensions
        fileName = fileName.replace(/-cumulative$/i, ''); // remove "-cumulative" if present
        return fileName.split(/[-_]/).map(word =>
            word.charAt(0).toUpperCase() + word.slice(1)
        ).join(' ');
    }

    const pageTitle = getTitleFromUrl(geojsonUrl);
    const mapScript = document.getElementById('geojson-map');
    const mapScriptUrl = mapScript && mapScript.src
        ? mapScript.src
        : document.baseURI;
    const baseMapUrl = new URL(
        'visionscarto-cahillkeyes-44x22.svg', mapScriptUrl
    ).href;

    return `<!DOCTYPE html>
<html lang="en">
<head>
    <title>${pageTitle}</title>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" />
    <style>
        html, body {
            height: 100%;
            margin: 0;
        }
        #map {
            height: 100vh;
            background: #f8f8f6;
        }
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
                <input type="number" id="min-radius" min="1" max="50" value="2" step="1">
                <span>pixels</span>
            </div>
            <div class="config-row">
                <label for="max-radius">Max radius:</label>
                <input type="number" id="max-radius" min="10" max="200" value="100" step="1">
                <span>pixels</span>
            </div>
            <div class="config-row">
                <label for="fill-opacity">Fill opacity:</label>
                <input type="number" id="fill-opacity" min="0" max="100" value="20" step="5">
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
        <div class="data-source">
            <strong>Projection:</strong>
            <a href="https://github.com/bdekoz/cartofreako"
               target="_blank" rel="noopener">cartofreako Cahill-Keyes</a>
            <br>
            <strong>Base map:</strong>
            <a href="https://visionscarto.net/"
               target="_blank" rel="noopener">Visionscarto</a>
        </div>

        <div class="note">Click circles for details • Zero values are hidden • 0.5pt stroke</div>
    </div>

    <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>

    <script>
${CartofreakoCahillKeyesProjection.toString()}

        // GeoJSON data source URL (passed in)
        const GEOJSON_URL = ${JSON.stringify(geojsonUrl)};
        const BASE_MAP_URL = ${JSON.stringify(baseMapUrl)};

        // ============================================================
        //  loadData() – fetches and decompresses if .gz / .gzip
        // ============================================================
        async function loadData() {
            const isGzip = GEOJSON_URL.endsWith('.gz') || GEOJSON_URL.endsWith('.gzip');
            const response = await fetch(GEOJSON_URL);
            if (!response.ok) throw new Error(\`HTTP \${response.status}\`);

            if (isGzip) {
                const blob = await response.blob();
                const ds = new DecompressionStream('gzip');
                const stream = blob.stream().pipeThrough(ds);
                const decompressedBlob = await new Response(stream).blob();
                const text = await decompressedBlob.text();
                return JSON.parse(text);
            } else {
                return await response.json();
            }
        }

        // Default settings
        const DEFAULT_CATEGORY = 'downloaders';
        const DEFAULT_PROPERTY = 'size';

        // The Visionscarto asset and cartofreako projection share this exact
        // 44 x 22 inch, 96-DPI logical frame.
        const MAP_WIDTH = 4224;
        const MAP_HEIGHT = 2112;
        const cahillKeyes = new CartofreakoCahillKeyesProjection(
            MAP_WIDTH, MAP_HEIGHT
        );
        const mapBounds = L.latLngBounds(
            [0, 0], [MAP_HEIGHT, MAP_WIDTH]
        );

        // Leaflet handles interaction in a flat coordinate space. Geographic
        // points are projected explicitly before they enter a Leaflet layer.
        const map = L.map('map', {
            crs: L.CRS.Simple,
            minZoom: -4,
            maxZoom: 2,
            zoomSnap: 0.25,
            zoomDelta: 0.5,
            maxBounds: mapBounds.pad(0.04),
            maxBoundsViscosity: 1,
            preferCanvas: true
        });

        L.imageOverlay(BASE_MAP_URL, mapBounds, {
            alt: 'Cahill-Keyes world map by Visionscarto',
            interactive: false
        }).addTo(map);
        map.attributionControl.addAttribution(
            'Projection: <a href="https://github.com/bdekoz/cartofreako"'
            + ' target="_blank" rel="noopener">cartofreako</a>; '
            + 'base map: <a href="https://visionscarto.net/"'
            + ' target="_blank" rel="noopener">Visionscarto</a>'
        );

        function resetCahillKeyesView() {
            map.fitBounds(mapBounds, {animate: false, padding: [8, 8]});
        }

        function projectLongitudeLatitude(longitude, latitude) {
            const point = cahillKeyes.project(latitude, longitude);
            // Leaflet's simple CRS grows upward; SVG/pixel y grows downward.
            return L.latLng(MAP_HEIGHT - point.y, point.x);
        }

        function geoJsonCoordinatesToLatLng(coordinates) {
            return projectLongitudeLatitude(coordinates[0], coordinates[1]);
        }

        resetCahillKeyesView();

        // Display data source
        document.getElementById('data-source').innerHTML = \`📁 <strong>Data:</strong> \${GEOJSON_URL.split('/').pop()}\`;

        // Configuration
        let config = {
            minRadius: 2,
            maxRadius: 100,
            fillOpacity: 0.2
        };

        let geoJsonData = null;
        let currentLayer = null;
        let currentCategory = DEFAULT_CATEGORY;
        let currentProperty = DEFAULT_PROPERTY;
        let originalFeatures = [];
        let currentReducedPoints = [];

        // Helper function to get property value (for new GeoJSON structure)
        function getPropertyValue(feature, category, property) {
            try {
                const val = feature.properties[category]?.[property];
                return val !== undefined && val !== null ? parseFloat(val) : 0;
            } catch (e) {
                console.warn(\`Error getting property \${category}.\${property}:\`, e);
                return 0;
            }
        }

        // Helper functions for distance and overlap
        function deg2km(lat) {
            const latKm = 111.32;
            const lngKm = 111.32 * Math.cos(lat * Math.PI / 180);
            return { latKm, lngKm };
        }

        function geographicCoordinates(point) {
            return point.geometry
                ? [point.geometry.coordinates[0], point.geometry.coordinates[1]]
                : [point.lng, point.lat];
        }

        function calculateDistance(point1, point2) {
            const [lng1, lat1] = geographicCoordinates(point1);
            const [lng2, lat2] = geographicCoordinates(point2);

            const { latKm, lngKm } = deg2km((lat1 + lat2) / 2);

            const dLat = (lat2 - lat1) * latKm;
            const longitudeDelta = ((lng2 - lng1 + 540) % 360) - 180;
            const dLng = longitudeDelta * lngKm;

            return Math.sqrt(dLat * dLat + dLng * dLng);
        }

        function weightedLongitude(first, firstWeight, second, secondWeight) {
            const firstRadians = first * Math.PI / 180;
            const secondRadians = second * Math.PI / 180;
            const x = Math.cos(firstRadians) * firstWeight
                + Math.cos(secondRadians) * secondWeight;
            const y = Math.sin(firstRadians) * firstWeight
                + Math.sin(secondRadians) * secondWeight;
            return Math.atan2(y, x) * 180 / Math.PI;
        }

        function circlesOverlap(point1, point2, value1, value2, minVal, maxVal) {
            const scaleFactor1 = maxVal > minVal ? (value1 - minVal) / (maxVal - minVal) : 0.5;
            const scaleFactor2 = maxVal > minVal ? (value2 - minVal) / (maxVal - minVal) : 0.5;

            const radiusPixels1 = config.minRadius + (config.maxRadius - config.minRadius) * scaleFactor1;
            const radiusPixels2 = config.minRadius + (config.maxRadius - config.minRadius) * scaleFactor2;

            const [lng1, lat1] = geographicCoordinates(point1);
            const [lng2, lat2] = geographicCoordinates(point2);
            const pixel1 = map.latLngToLayerPoint(
                projectLongitudeLatitude(lng1, lat1)
            );
            const pixel2 = map.latLngToLayerPoint(
                projectLongitudeLatitude(lng2, lat2)
            );

            return pixel1.distanceTo(pixel2)
                < radiusPixels1 + radiusPixels2;
        }

        function filterFeaturesByValue(features, category, property) {
            return features.filter(f => {
                const val = getPropertyValue(f, category, property);
                return !isNaN(val) && val >= 1;
            });
        }

        function reducePointsToEliminateOverlaps(features, category, property, maxDistance) {
            if (features.length === 0) return [];

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
                            minVal, maxVal
                        )) {
                            const totalValue = points[i].value + points[j].value;

                            points[i].lat = (points[i].lat * points[i].value + points[j].lat * points[j].value) / totalValue;
                            points[i].lng = weightedLongitude(
                                points[i].lng, points[i].value,
                                points[j].lng, points[j].value
                            );
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
                coordsToLatLng: geoJsonCoordinatesToLatLng,

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
                            origValue = origProps[currentCategory]?.[currentProperty];
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
                map.fitBounds(currentLayer.getBounds(), {
                    paddingTopLeft: [20, 20],
                    paddingBottomRight: [350, 20],
                    maxZoom: 0.5
                });
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

        // ============================================================
        // Load data and initialise map
        // ============================================================
        loadData()
            .then(data => {
                geoJsonData = data;
                originalFeatures = data.features;

                console.log('First feature:', originalFeatures[0]);
                console.log('Downloaders size:', originalFeatures[0].properties.downloaders?.size);
                console.log('Uploaders size:', originalFeatures[0].properties.uploaders?.size);

                // Set default category buttons
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

        // ============================================================
        // Event listeners
        // ============================================================
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
            resetCahillKeyesView();
        });
    </script>
</body>
</html>`;
}


// ============================================================
// Open a new window with the map – pass the URL only
// ============================================================
function leaflet_map_open_window(geojsonUrl, title) {
    const htmlContent = leaflet_map_geojson(geojsonUrl);
    const windowspec = 'width=1200,height=800,resizable=yes,scrollbars=yes';
    const newWindow = window.open('', '_blank', windowspec);
    if (!newWindow) {
        throw new Error('The map window was blocked by the browser');
    }
    newWindow.document.write(htmlContent);
    newWindow.document.title = title || 'Map';
    newWindow.document.close();
}
