const webpack = require("webpack");

module.exports = {
  entry: "./src/index.js",
  output: {
    path: __dirname + "/build/js",
    filename: "main.js",
    sourceMapFilename: "[file].map",
    devtoolModuleFilenameTemplate: (info) =>
      `webpack:///${info.absoluteResourcePath.replace(/\\/g, "/")}`,
  },
  // Required for duktape to ensure webpack chunk do not contain arrow functions
  target: ["web", "es5"],
  devtool: "source-map",
  module: {
    rules: [
      {
        test: /\.(js|jsx)$/,
        exclude: /node_modules/,
        use: ["babel-loader"],
      },
      {
        test: /\.svg$/,
        exclude: /node_modules/,
        use: ["svg-inline-loader"],
      },
      {
        test: /\.(png|jpeg|jpg|gif)$/,
        use: [
          {
            loader: "url-loader",
            options: {
              limit: true,
              esModule: false,
            },
          },
        ],
      },
    ],
  },
  plugins: [
    new webpack.DefinePlugin({
      "process.env.NODE_DEBUG": JSON.stringify(process.env.NODE_DEBUG),
    }),
  ],
};
