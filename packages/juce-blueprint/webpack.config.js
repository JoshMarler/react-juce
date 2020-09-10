const path = require('path');

module.exports = env => {
    return {
        entry: './src/index.tsx',
        output: {
          path: __dirname + '/dist',
          filename: 'index.js',
          libraryTarget: "umd",
          globalObject: "this",
          sourceMapFilename: "[file].map",
          devtoolModuleFilenameTemplate: 'webpack:///[absolute-resource-path]'
        },
        externals: {
          react: {
            root: 'React',
            commonjs2: 'react',
            commonjs: 'react',
            amd: 'react'
          },
        },
        devtool: "source-map",
        resolve: {
          extensions: [".ts", ".tsx", ".js"]
        },
        module: {
          rules: [
            {
              test: /\.tsx?$/,
              exclude: /node_modules/,
              use: [
                {
                  loader: "awesome-typescript-loader",
                  options: {
                     useBabel: true,
                     babelCore: '@babel/core',
                     declarationDir: __dirname + '/dist'
                  }
                }
              ]
            },
            {
              test: /\.svg$/,
              exclude: /node_modules/,
              use: ['svg-inline-loader']
            },
          ]
        },
        watchOptions: {
          ignored: [
            path.resolve(__dirname, 'dist'),
            path.resolve(__dirname, 'node_modules'),
          ]
        }
    }
};
