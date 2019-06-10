/** Constructs and returns a Path data array for drawing a border in the given
 *  dimensions.
 *
 *  @param {number} x1
 *  @param {number} y1
 *  @param {number} width
 *  @param {number} height
 *  @param {number} rtl : border radius top left corner
 *  @param {number} rtr : border radius top right corner
 *  @param {number} rbr : border radius bottom right corner
 *  @param {number} rbl : border radius bottom left corner
 */
export function drawBorderPath(x1, y1, width, height, rtl, rtr, rbr, rbl) {
  let dataArray = [];
  let x2 = x1 + width;
  let y2 = y1 + height;

  // Magnitudes
  let artl = Math.abs(rtl);
  let artr = Math.abs(rtr);
  let arbr = Math.abs(rbr);
  let arbl = Math.abs(rbl);

  // Sweep flags (whether to notch or round the corner)
  let ntl = ~~(rtl >= 0.0);
  let ntr = ~~(rtr >= 0.0);
  let nbr = ~~(rbr >= 0.0);
  let nbl = ~~(rbl >= 0.0);

  // Top left corner
  dataArray.push(`M ${x1} ${y1 + (y2 - y1) * 0.5}`);
  dataArray.push(`L ${x1} ${y1 + artl}`);
  dataArray.push(`A ${artl} ${artl} 0 0 ${ntl} ${x1 + artl} ${y1}`);

  // Top right corner
  dataArray.push(`L ${x2 - artr} ${y1}`);
  dataArray.push(`A ${artr} ${artr} 0 0 ${ntr} ${x2} ${y1 + artr}`);

  // Bottom right corner
  dataArray.push(`L ${x2} ${y2 - arbr}`);
  dataArray.push(`A ${arbr} ${arbr} 0 0 ${nbr} ${x2 - arbr} ${y2}`);

  // Bottom left corner
  dataArray.push(`L ${x1 + arbl} ${y2}`);
  dataArray.push(`A ${arbl} ${arbl} 0 0 ${nbl} ${x1} ${y2 - arbl}`);

  // Close path
  dataArray.push(`Z`);

  // Path length
  let length = 0.5 * Math.PI * (artl + artr + arbr + arbl) +
    (width - artl - artr) + (width - arbr - arbl) +
    (height - artr - arbr) + (height - artl - arbl);

  return {
    border: dataArray,
    length: length,
  };
}

export function drawGrainFrequencyPattern(value, width, height) {
  let pathData = [];
  let pathData2 = [];
  let strokeWidth = 2.0;
  let halfStrokeWidth = 1.0;

  let cy = height * 0.5;
  pathData.push(`M 0 ${cy}`);
  pathData2.push(`M 0 ${cy}`);

  let vv = value * value * value * value * 4.0;

  for (let x = 0; x < width; x++) {
    let y1 = cy + 20 * Math.cos(4.0 * (vv + 0.12) * Math.PI * (x / width));
    let y2 = cy + 20 * Math.cos(Math.PI + 4.0 * (vv + 0.12) * Math.PI * (x / width));

    pathData.push(`L ${x} ${y1}`);
    pathData2.push(`L ${x} ${y2}`);
  }

  return `
    <path d="${pathData.join(' ')}" stroke="#66FDCF" stroke-width="${strokeWidth}" />
    <path d="${pathData2.join(' ')}" stroke="#62E7FD" stroke-width="${strokeWidth}" />
  `;
}

export function drawGrainSprayPattern(value, width, height) {
  let xStep = width / 16.0;
  let yStep = height / 16.0;
  let strokeWidth = 2.0;
  let maxOffset = 8.0;
  let pathData = [];

  for (let x = 0; x < width; x += xStep) {
    for (let y = 5.0 * yStep; y < 12.0 * yStep; y += yStep) {
      let xr = x + value * Math.random() * maxOffset;
      let yr = y + value * Math.random() * maxOffset;
      pathData.push(`M ${xr} ${yr}`);
      pathData.push(`L ${xr + 1.0} ${yr}`);
    }
  }

  return `
    <path d="${pathData.join(' ')}" stroke="#66FDCF" stroke-width="${strokeWidth}" />
  `;
}

const spreadRand = [];

export function drawGrainSpreadPattern(value, width, height) {
  let x1 = 0.48 * width;
  let x2 = 0.52 * width;
  let numSteps = 16;
  let yStep = height / numSteps;
  let maxOffset = 16.0;
  let strokeWidth = 2.0;
  let pathData = [];

  if (spreadRand.length !== numSteps || value < 0.001) {
    for (let i = 0; i < numSteps; ++i) {
      spreadRand[i] = Math.random();
    }
  }

  for (let i = 2; i < numSteps - 2; ++i) {
    let y = i * yStep;

    pathData.push(`M ${x1 - maxOffset * value * spreadRand[i]} ${y}`);
    pathData.push(`L ${x1 - maxOffset * value * spreadRand[i]} ${y + yStep * 0.8}`);
    pathData.push(`M ${x2 + maxOffset * value * spreadRand[i]} ${y}`);
    pathData.push(`L ${x2 + maxOffset * value * spreadRand[i]} ${y + yStep * 0.8}`);
  }

  return `
    <path d="${pathData.join(' ')}" stroke="#62E7FD" stroke-width="${strokeWidth}" />
  `;
}

export function drawGrainPitchPattern(value, width, height) {
  let xStep = width / 10.0;
  let yStep = height / 12.0;
  let strokeWidth = 2.0;
  let maxOffset = 18.0;
  let pathData = [];
  let pathData2 = [];
  let rand = 8.0;

  // Quantize the value to 24 steps, centered about zero, on the range [-1, 1]
  let qValue = (Math.round(value * 24) / 24) * 2.0 - 1.0;

  for (let x = 0; x < width; x += xStep) {
    for (let y = 4.0 * yStep; y < 8.0 * yStep; y += yStep) {
      let xr = x;
      let yr = y - qValue * (x / width) * maxOffset;
      pathData.push(`M ${xr} ${y + (yr - y) * 0.5}`);
      pathData.push(`L ${xr + xStep * 0.8} ${yr}`);

      pathData2.push(`M ${xr + rand} ${rand + y + (yr - y) * 0.5}`);
      pathData2.push(`L ${rand + xr + xStep * 0.8} ${rand + yr}`);
    }
  }

  return `
    <path d="${pathData.join(' ')}" stroke="#66FDCF" stroke-width="${strokeWidth}" stroke-opacity="0.8" />
    <path d="${pathData2.join(' ')}" stroke="#62E7FD" stroke-width="${strokeWidth}" stroke-opacity="0.5" />
  `;
}

const reverseRandCoords = [];

export function drawGrainReversePattern(value, width, height) {
  let strokeWidth = 2.0;
  let numGrains = 32;
  let tw = width / 16.0;
  let th = tw;

  if (reverseRandCoords.length !== numGrains) {
    for (let i = 0; i < numGrains; ++i) {
      reverseRandCoords.push({
        x: Math.random(),
        y: 0.2 + 0.6 * Math.random(),
        opacity: Math.random(),
        color: Math.random() < 0.5 ? "#66FDCF" : "#62E7FD",
        r: Math.random(),
      });
    }
  }

  return reverseRandCoords.map(function(p) {
    let x = p.x * width;
    let y = p.y * height;
    let rev = (p.r < value) ? -1 : 1;

    let line = `M ${x} ${y} L ${x + rev * tw} ${y + th * 0.5} L ${x} ${y + th} Z`;

    return `<path d="${line}" stroke="${p.color}" stroke-width="${strokeWidth}" stroke-opacity="${p.opacity}" />`
  }).join('\n');
}

export function drawGrainFeedbackPattern(value, width, height) {
  let numRings = 8;
  let numSteps = 32;
  let strokeWidth = 2.0;
  let maxOffset = 2.0;
  let cx = width * 0.5;
  let cy = height * 0.5;
  let rx = Math.min(width, height) * 0.25;
  let paths = [];

  for (let i = 0; i < numRings; ++i) {
    let pathData = [`M ${cx - rx} ${cy}`];

    for (let j = 0; j < numSteps; ++j) {
      let angle = j * 2.0 * Math.PI / numSteps;

      let x = cx - Math.cos(value * angle) * rx - value * angle * Math.random() * maxOffset;
      let y = cy - Math.sin(value * angle) * rx - value * angle * Math.random() * maxOffset;

      pathData.push(`L ${x} ${y}`);
    }

    paths.push(`
      <path d="${pathData.join(' ')}" stroke="#62E7FD" stroke-width="${strokeWidth}" stroke-opacity="${Math.random() * 0.5}"/>
    `);
  }

  return paths.join('\n');
}
