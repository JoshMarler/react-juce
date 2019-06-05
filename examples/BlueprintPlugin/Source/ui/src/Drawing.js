/** Constructs and returns a Path data array for drawing a border in the given
 *  dimensions.
 *
 *  @param {number} width
 *  @param {number} height
 *  @param {number} rtl : border radius top left corner
 *  @param {number} rtr : border radius top right corner
 *  @param {number} rbr : border radius bottom right corner
 *  @param {number} rbl : border radius bottom left corner
 */
export function drawBorderPath(width, height, rtl, rtr, rbr, rbl) {
  // TODO: Take an x,y prop as well so that the caller can offset the border
  // from the edges of the component if it wants (for the juce path border trick)
  let dataArray = [];

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
  dataArray.push(`M ${0} ${artl}`);
  dataArray.push(`A ${artl} ${artl} 0 0 ${ntl} ${artl} ${0}`);

  // Top right corner
  dataArray.push(`L ${width - artr} ${0}`);
  dataArray.push(`A ${artr} ${artr} 0 0 ${ntr} ${width} ${artr}`);

  // Bottom right corner
  dataArray.push(`L ${width} ${height - arbr}`);
  dataArray.push(`A ${arbr} ${arbr} 0 0 ${nbr} ${width - arbr} ${height}`);

  // Bottom left corner
  dataArray.push(`L ${arbl} ${height}`);
  dataArray.push(`A ${arbl} ${arbl} 0 0 ${nbl} ${0} ${height - arbl}`);

  // Close path
  dataArray.push(`Z`);

  return dataArray;
}
