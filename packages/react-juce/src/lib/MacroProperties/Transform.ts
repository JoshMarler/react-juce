import { TPropertyAssignment } from "./types";
import { getMacroCalls } from "./util";
import matrix from "matrix-js";

const rotateMultipliers = {
  deg: Math.PI / 180.0,
  grad: Math.PI / 200.0,
  rad: 1,
  turn: 2 * Math.PI,
};

const angleUnitMatcher = new RegExp(
  `(-?[0-9]*\.?[0-9]+)(${Object.keys(rotateMultipliers).join("|")}|)$`
);

const toRadians = (arg: string) => {
  // @ts-ignore
  let [, angle, unit] = angleUnitMatcher.exec(arg);
  let angleFloat = parseFloat(angle);
  if (angleFloat === NaN) return NaN;
  if (unit === "" && angleFloat !== 0) return NaN;
  angleFloat *= rotateMultipliers[unit] || 1;
  return angleFloat;
};

const stringArgsToFloat = (...args: string[]) => args.map(parseFloat);
const stringArgsToRadians = (...args: string[]) => args.map(toRadians);

const identity = matrix([
  [1, 0, 0, 0],
  [0, 1, 0, 0],
  [0, 0, 1, 0],
  [0, 0, 0, 1],
]);

const rotate3d = (x: number, y: number, z: number, theta: number) => {
  const sinTheta = Math.sin(theta);
  const cosTheta = Math.cos(theta);
  const r = Math.sqrt(x ** 2 + y ** 2 + z ** 2);
  if (r === 0) {
    return identity;
  }
  const [u, v, w] = [x / r, y / r, z / r];

  return matrix([
    [
      cosTheta + (1 - cosTheta) * u ** 2,
      u * v * (1 - cosTheta) - w * sinTheta,
      u * w * (1 - cosTheta) + v * sinTheta,
      0,
    ],
    [
      v * u * (1 - cosTheta) + w * sinTheta,
      cosTheta + (1 - cosTheta) * v ** 2,
      v * w * (1 - cosTheta) - u * sinTheta,
      0,
    ],
    [
      w * u * (1 - cosTheta) - v * sinTheta,
      w * v * (1 - cosTheta) + u * sinTheta,
      cosTheta + (1 - cosTheta) * w ** 2,
      0,
    ],
    [0, 0, 0, 1],
  ]);
};

const rotateX = (theta: number) => rotate3d(1, 0, 0, theta);
const rotateY = (theta: number) => rotate3d(0, 1, 0, theta);
const rotateZ = (theta: number) => rotate3d(0, 0, 1, theta);

const scale3d = (sx = 1, sy = 1, sz = 1) =>
  matrix([
    [sx, 0, 0, 0],
    [0, sy, 0, 0],
    [0, 0, sz, 0],
    [0, 0, 0, 1],
  ]);

const scale = (sx = 1, sy = 1) => scale3d(sx, sy);
const scaleX = (sx = 1) => scale(sx);
const scaleY = (sy = 1) => scale(1, sy);
const scaleZ = (sz = 1) => scale3d(1, 1, sz);

const skew = (alpha = 0, beta = 0) => {
  const tanAlpha = Math.tan(alpha) || 0;
  const tanBeta = Math.tan(beta) || 0;
  return matrix([
    [1 + tanAlpha * tanBeta, tanAlpha, 0, 0],
    [tanBeta, 1, 0, 0],
    [0, 0, 1, 0],
    [0, 0, 0, 1],
  ]);
};

const skewX = (theta: number) => skew(theta);
const skewY = (theta: number) => skew(0, theta);

const translate3d = (dx = 0, dy = 0, dz = 0) =>
  matrix([
    [1, 0, 0, dx],
    [0, 1, 0, dy],
    [0, 0, 1, dz],
    [0, 0, 0, 1],
  ]);

const translate = (dx = 0, dy = 0) => translate3d(dx, dy, 0);
const translateX = (dx = 0) => translate3d(dx);
const translateY = (dy = 0) => translate3d(0, dy);
const translateZ = (dz = 0) => translate3d(0, 0, dz);

const argsTransformMap = {
  matrix: stringArgsToFloat,
  matrix3d: stringArgsToFloat,
  rotate: stringArgsToRadians,
  rotate3d: (x, y, z, t) => [...stringArgsToFloat(x, y, z), toRadians(t)],
  rotateX: stringArgsToRadians,
  rotateY: stringArgsToRadians,
  rotateZ: stringArgsToRadians,
  scale: stringArgsToFloat,
  scale3d: stringArgsToFloat,
  scaleX: stringArgsToFloat,
  scaleY: stringArgsToFloat,
  scaleZ: stringArgsToFloat,
  skew: stringArgsToRadians,
  skewX: stringArgsToRadians,
  skewY: stringArgsToRadians,
  translate: stringArgsToFloat,
  translate3d: stringArgsToFloat,
  translateX: stringArgsToFloat,
  translateY: stringArgsToFloat,
  translateZ: stringArgsToFloat,
};

const transformMatrixMap = {
  none: identity,
  matrix: (l11, l12, dx, l21, l22, dy) =>
    matrix([
      [l11, l12, 0, dx],
      [l21, l22, 0, dy],
      [0, 0, 1, 0],
      [0, 0, 0, 1],
    ]),
  matrix3d: (
    m11,
    m12,
    m13,
    m14,
    m21,
    m22,
    m23,
    m24,
    m31,
    m32,
    m33,
    m34,
    m41,
    m42,
    m43,
    m44
  ) =>
    matrix([
      [m11, m12, m13, m14],
      [m21, m22, m23, m24],
      [m31, m32, m33, m34],
      [m41, m42, m43, m44],
    ]),
  // perspective: ()=>identity, // TODO
  rotate: rotateZ,
  rotateX,
  rotateY,
  rotateZ,
  scale,
  scale3d,
  scaleX,
  scaleY,
  scaleZ,
  skew,
  skewX,
  skewY,
  translate,
  translate3d,
  translateX,
  translateY,
  translateZ,
};

const matrixToArray = (m) => m().reduce((acc, v) => [...acc, ...v]);

export default function (value: string): TPropertyAssignment[] {
  const calls = getMacroCalls(value);
  // operations performed right to left
  // i.e. transform: translateY(20) rotate(90deg)
  // means rotate 90 degrees, then translate up 20
  // so we reverse in-place the calls array which initially
  // has the translation first followed by the rotate.
  calls.reverse();
  //@ts-ignore
  const transformMatrix = calls.reduce((acc, { macro: f, args }) => {
    if (!transformMatrixMap.hasOwnProperty(f)) {
      return acc;
    }
    const argsTransform = argsTransformMap[f] || (() => args);
    const transform = transformMatrixMap[f](...argsTransform(...args));
    return matrix(transform.prod(acc));
  }, identity);
  return [["transform-matrix", matrixToArray(transformMatrix)]];
}
