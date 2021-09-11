const path = require("path");

module.exports = (env) => {
  return {
    entry: "./src/index.tsx",
    output: {
      path: path.resolve(__dirname, "dist"),
      filename: "index.js",
      libraryTarget: "umd",
      globalObject: "this",
      sourceMapFilename: "[file].map",
      devtoolModuleFilenameTemplate: "webpack:///[absolute-resource-path]",
    },
    externals: {
      react: {
        root: "React",
        commonjs2: "react",
        commonjs: "react",
        amd: "react",
      },
    },
    target: ["web", "es5"],
    devtool: "source-map",
    resolve: {
      extensions: [".ts", ".tsx", ".js"],
    },
    module: {
      rules: [
        {
          test: /\.tsx?$/,
          exclude: path.resolve(__dirname, "node_modules"),
          use: [
            {
              loader: "awesome-typescript-loader",
              options: {
                useBabel: true,
                babelCore: "@babel/core",
                declarationDir: path.resolve(__dirname, "dist"),
              },
            },
          ],
        },
        {
          test: /\.js$/,
          include: [
            path.resolve(__dirname, "node_modules", "matrix-js"),
            path.resolve(__dirname, "node_modules", "camelcase"),
          ],
          use: [
            {
              loader: "awesome-typescript-loader",
              options: {
                useBabel: true,
                babelCore: "@babel/core",
              },
            },
          ],
        },
        {
          test: /\.svg$/,
          exclude: path.resolve(__dirname, "node_modules"),
          use: ["svg-inline-loader"],
        },
      ],
    },
    watchOptions: {
      ignored: [
        path.resolve(__dirname, "dist"),
        path.resolve(__dirname, "node_modules"),
      ],
    },
  };
};
