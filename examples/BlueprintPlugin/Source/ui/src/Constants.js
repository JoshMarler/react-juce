export const Colors = {
  BACKGROUND: 'ff17191f',
  STROKE: 'ff626262',
  DBG_RED: 'ff884848',
  DBG_GREEN: 'ff488848',
  TRANSPARENT: '00000000',
  HIGHLIGHT: 'ff66FDCF',
  SOFT_HIGHLIGHT: '7766FDCF',
};

export const ParamIds = {
  DELAY: 'DelayMs',
  WARP: 'Warp',
  FILTER_CUTOFF: 'Cutoff',
  FILTER_TYPE: 'FilterType',

  GRAIN_FREQUENCY: 'GrainFrequency',
  SPRAY: 'Spray',
  REVERSE: 'Reverse',
  SPREAD: 'Spread',
  PITCH: 'Pitch',
  FEEDBACK_G1G2: 'FeedbackG1G2',

  GRAIN_FREQUENCY_G2: 'GrainFrequencyG2',
  SPRAY_G2: 'SprayG2',
  REVERSE_G2: 'ReverseG2',
  SPREAD_G2: 'SpreadG2',
  PITCH_G2: 'PitchG2',
  FEEDBACK_G2G1: 'FeedbackG2G1',

  FREEZE: 'Freeze',
  ENVELOPE_THRESHOLD: 'EnvelopeThreshold',

  WET_AMP: 'WetAmp',
  DRY_AMP: 'DryAmp',
  MIX: 'Mix',
};

export const ParamLabels = {
  [ParamIds.DELAY]: 'DELAY',
  [ParamIds.WARP]: 'WARP',
  [ParamIds.FILTER_CUTOFF]: 'CUTOFF',
  [ParamIds.ENVELOPE_THRESHOLD]: 'THRESHOLD',
  [ParamIds.MIX]: 'MIX',
};
