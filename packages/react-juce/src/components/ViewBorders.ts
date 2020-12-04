import invariant from 'invariant';

type BorderColors = {
  borderTopColor: string | undefined,
  borderRightColor: string | undefined,
  borderBottomColor: string | undefined,
  borderLeftColor: string | undefined
}

type BorderWidths = {
  borderTopWidth: number | undefined,
  borderRightWidth: number | undefined,
  borderBottomWidth: number | undefined,
  borderLeftWidth: number | undefined,
}

type BorderRadii = {
  borderTopRightRadius: number | undefined,
  borderBottomRightRadius: number | undefined,
  borderBottomLeftRadius: number | undefined,
  borderTopLeftRadius: number | undefined
}

function parseBorderWidthString(borderWidthString: string): BorderWidths {
  const values = borderWidthString.split(' ');

  invariant(
    values.length > 0 && values.length < 5,
    ''
  );

  if (values.length === 1) {
    const width = parseFloat(values[0])
    return {
      borderTopWidth: width,
      borderRightWidth: width,
      borderBottomWidth: width,
      borderLeftWidth: width
    }
  }

  if (values.length === 2) {
    return {
      borderTopWidth: parseFloat(values[0]),
      borderRightWidth: parseFloat(values[1]),
      borderBottomWidth: parseFloat(values[1]),
      borderLeftWidth: parseFloat(values[0])
    }
  }

  if (values.length === 3) {
    return {
      borderTopWidth: parseFloat(values[0]),
      borderRightWidth: parseFloat(values[1]),
      borderBottomWidth: parseFloat(values[0]),
      borderLeftWidth: parseFloat(values[1])
    }
  }

  if (values.length === 4 ) {
    return {
      borderTopWidth: parseFloat(values[0]),
      borderRightWidth: parseFloat(values[1]),
      borderBottomWidth: parseFloat(values[2]),
      borderLeftWidth: parseFloat(values[3])
    }
  }

  return {
    borderTopWidth: undefined,
    borderRightWidth: undefined,
    borderBottomWidth: undefined,
    borderLeftWidth: undefined
  };
}

export function parseBorderWidthProp(borderWidthProp: string | number): BorderWidths {
  if (typeof borderWidthProp === 'number') {
    return {
      borderTopWidth: borderWidthProp,
      borderRightWidth: borderWidthProp,
      borderBottomWidth: borderWidthProp,
      borderLeftWidth: borderWidthProp
    };
  }
  else {
    return parseBorderWidthString(borderWidthProp);
  }
}

export function parseBorderWidthProps(props: any) {
  return {
    borderTopWidth: props['border-top-width'],
    borderRightWidth: props['border-right-width'],
    borderBottomWidth: props['border-bottom-width'],
    borderLeftWidth: props['border-left-width']
  };
}

function parseBorderRadiusString(borderRadiusString: string): BorderRadii {
  const values = borderRadiusString.split(' ');

  invariant(
    values.length > 0 && values.length < 5,
    ''
  );

  if (values.length === 1) {
    const radius = parseFloat(values[0]);
    return {
      borderTopRightRadius: radius,
      borderBottomRightRadius: radius,
      borderBottomLeftRadius: radius,
      borderTopLeftRadius: radius
    };
  }

  if (values.length === 2) {
    return {
      borderTopRightRadius: parseFloat(values[1]),
      borderBottomRightRadius: parseFloat(values[0]),
      borderBottomLeftRadius: parseFloat(values[1]),
      borderTopLeftRadius: parseFloat(values[0])
    };
  }

  if (values.length === 3) {
    return {
      borderTopRightRadius: parseFloat(values[1]),
      borderBottomRightRadius: parseFloat(values[3]),
      borderBottomLeftRadius: parseFloat(values[1]),
      borderTopLeftRadius: parseFloat(values[0])
    };
  }

  if (values.length === 4 ) {
    return {
      borderTopRightRadius: parseFloat(values[1]),
      borderBottomRightRadius: parseFloat(values[2]),
      borderBottomLeftRadius: parseFloat(values[3]),
      borderTopLeftRadius: parseFloat(values[0])
    };
  }

  return {
    borderTopRightRadius: undefined,
    borderBottomRightRadius: undefined,
    borderBottomLeftRadius: undefined,
    borderTopLeftRadius: undefined
  };
}

export function parseBorderRadiusProp(borderRadiusProp: string | number) {
  if (typeof borderRadiusProp === 'number') {
    return {
      borderTopRightRadius: borderRadiusProp,
      borderBottomRightRadius: borderRadiusProp,
      borderBottomLeftRadius: borderRadiusProp,
      borderTopLeftRadius: borderRadiusProp
    };
  }
  else {
    return parseBorderRadiusString(borderRadiusProp);
  }
}

export function parseBorderRadiusProps(props: any) {
  return {
    borderTopRightRadius: props['border-top-right-radius'],
    borderBottomRightRadius: props['border-bottom-right-radius'],
    borderBottomLeftRadius: props['border-bottom-left-radius'],
    borderTopLeftRadius: props['border-top-left-radius']
  };
}

export function parseBorderColorProp(borderColorProp: string): BorderColors {
  const values = borderColorProp.split(' ');

  invariant(
    values.length > 0 && values.length < 5,
    ''
  );

  if (values.length === 1){
    const color = values[0];
    return {
      borderTopColor: color,
      borderRightColor: color,
      borderBottomColor: color,
      borderLeftColor: color
    };
  }

  if (values.length === 2) {
    return {
      borderTopColor: values[0],
      borderRightColor: values[1],
      borderBottomColor: values[0],
      borderLeftColor: values[1]
    };
  }

  if (values.length === 3) {
    return {
      borderTopColor: values[0],
      borderRightColor: values[1],
      borderBottomColor: values[2],
      borderLeftColor: values[1]
    };
  }

  if (values.length === 4 ) {
    return {
      borderTopColor: values[0],
      borderRightColor: values[1],
      borderBottomColor: values[2],
      borderLeftColor: values[1]
    };
  }

  return {
    borderTopColor: undefined,
    borderRightColor: undefined,
    borderBottomColor: undefined,
    borderLeftColor: undefined
  };
}

export function parseBorderColorProps(props: any) {
  return {
    borderTopColor: props['border-top-color'],
    borderRightColor: props['border-right-color'],
    borderBottomColor: props['border-bottom-color'],
    borderLeftColor: props['border-left-color']
  };
}

export function buildBorderSVG(width: number,
                               height: number,
                               backgroundColor: string,
                               borderColors: BorderColors,
                               borderWidths: BorderWidths,
                               borderRadii: BorderRadii) {
  const x1 = 0;
  const y1 = 0;
  let x2 = x1 + width;
  let y2 = y1 + height;

  const topColor = typeof borderColors.borderTopColor !== 'undefined' ? borderColors.borderTopColor.substring(2) : "";
  const rightColor = typeof borderColors.borderRightColor !== 'undefined' ? borderColors.borderRightColor.substring(2) : "";
  const bottomColor = typeof borderColors.borderBottomColor !== 'undefined' ? borderColors.borderBottomColor.substring(2) : "";
  const leftColor = typeof borderColors.borderLeftColor !== 'undefined' ? borderColors.borderLeftColor.substring(2) : "";

  const topWidth = typeof borderWidths.borderTopWidth !== 'undefined' ? borderWidths.borderTopWidth : 0.0;
  const rightWidth = typeof borderWidths.borderRightWidth !== 'undefined' ? borderWidths.borderRightWidth : 0.0;
  const bottomWidth = typeof borderWidths.borderBottomWidth !== 'undefined' ? borderWidths.borderBottomWidth : 0.0;
  const leftWidth = typeof borderWidths.borderLeftWidth !== 'undefined' ? borderWidths.borderLeftWidth : 0.0;

  const rtr = typeof borderRadii.borderTopRightRadius !== 'undefined' ? borderRadii.borderTopRightRadius : 0.0;
  const rtl = typeof borderRadii.borderTopLeftRadius !== 'undefined' ? borderRadii.borderTopLeftRadius : 0.0;
  const rbr = typeof borderRadii.borderBottomRightRadius !== 'undefined' ? borderRadii.borderBottomRightRadius : 0.0;
  const rbl = typeof borderRadii.borderBottomLeftRadius !== 'undefined' ? borderRadii.borderBottomLeftRadius : 0.0;

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

  //============================================================================
  let topPathCommands: any[] = [];
  topPathCommands.push(`M ${x1 + artl} ${y1}`);
  topPathCommands.push(`L ${x2 - artr} ${y1}`);

  const topPath = `
    <path 
      d="${topPathCommands.join(' ')}" 
      stroke="#${topColor}" 
      stroke-width="${topWidth}" />
  `;

  //============================================================================
  let rightPathCommands: any[] = [];
  rightPathCommands.push(`M ${x2} ${y1 + artr}`);
  rightPathCommands.push(`L ${x2} ${y2 - arbr}`);

  const rightPath = `
    <path 
      d="${rightPathCommands.join(' ')}" 
      stroke="#${rightColor}" 
      stroke-width="${rightWidth}" />
  `;

  //============================================================================
  let bottomPathCommands: any[] = [];
  bottomPathCommands.push(`M ${x1 + arbl} ${y2}`);
  bottomPathCommands.push(`L ${x2 - arbr} ${y2}`);

  const bottomPath = `
    <path 
      d="${bottomPathCommands.join(' ')}" 
      stroke="#${bottomColor}" 
      stroke-width="${bottomWidth}" />
  `;

  //============================================================================
  let leftPathCommands: any[] = [];
  leftPathCommands.push(`M ${x1} ${y1 + artl}`);
  leftPathCommands.push(`L ${x1} ${y2 - arbl}`);

  const leftPath = `
    <path 
      d="${leftPathCommands.join(' ')}" 
      stroke="#${leftColor}" 
      stroke-width="${leftWidth}" />
  `;

  //============================================================================
  let rtlPathCommands: any[] = [];
  rtlPathCommands.push(`M ${x1} ${y1 + artl}`);
  rtlPathCommands.push(`A ${artl} ${artl} 0 0 ${ntl} ${x1 + artl} ${y1}`);

  //TODO: Which color to take? Do we always take color from left-right or do we
  //      draw the arc with two separate paths?
  //TODO: If left width is 0 try top
  //TODO: If left color is 0 try top

  const rtlPath = `
    <path 
      d="${rtlPathCommands.join(' ')}"
      stroke="#${leftColor}"
      stroke-width="${leftWidth}"
    />
  `;

  //============================================================================
  let rtrPathCommands: any[] = [];
  rtrPathCommands.push(`M ${x2 - artr} ${y1}`);
  rtrPathCommands.push(`A ${artr} ${artr} 0 0 ${ntr} ${x2} ${y1 + artr}`);

  const rtrPath = `
    <path 
      d="${rtrPathCommands.join(' ')}"
      stroke="#${topColor}"
      stroke-width="${topWidth}"
    />
  `;

  //============================================================================
  let rbrPathCommands: any[] = [];
  rbrPathCommands.push(`M ${x2} ${y2 - arbr}`);
  rbrPathCommands.push(`A ${arbr} ${arbr} 0 0 ${nbr} ${x2 - arbr} ${y2}`);

  const rbrPath = `
    <path 
      d="${rbrPathCommands.join(' ')}"
      stroke="#${rightColor}"
      stroke-width="${rightWidth}"
    />
  `;

  //============================================================================
  let rblPathCommands: any[] = [];
  rblPathCommands.push(`M ${x1 + arbl} ${y2}`);
  rblPathCommands.push(`A ${arbl} ${arbl} 0 0 ${nbl} ${x1} ${y2 - arbl}`);

  const rblPath = `
    <path 
      d="${rblPathCommands.join(' ')}"
      stroke="#${bottomColor}"
      stroke-width="${bottomWidth}"
    />
  `;

  //============================================================================

  //TODO: Add clipping path and fill inside with background-color prop

  return `
    <svg width="${width}" height="${height}">
       ${rtlPath}
       ${rtrPath}
       ${rbrPath}
       ${rblPath}
       ${topPath} 
       ${rightPath}
       ${bottomPath}
       ${leftPath}
    </svg>
  `;
}
