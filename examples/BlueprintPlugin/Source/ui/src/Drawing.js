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
