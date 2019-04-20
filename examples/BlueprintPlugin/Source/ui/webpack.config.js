module.exports = {
  entry: './src/index.js',
  output: {
    path: __dirname + '/build/js',
    filename: 'main.js'
  },
  module: {
    rules: [
      {
        test: /\.(js|jsx)$/,
        exclude: /node_modules/,
        use: ['babel-loader']
      },
      {
        test: /\.svg$/,
        exclude: /node_modules/,
        use: ['svg-inline-loader']
      },
    ]
  },
};
