const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');

module.exports = {
  entry: './public/index.js',
  output: {
    filename: 'bundle.js',
    path: path.resolve(__dirname, 'data'),
    assetModuleFilename: '[name][ext]'
  },
  module: {
    rules: [
      {
        test: /\.js$/, // Include .js files
        exclude: /node_modules/, // Exclude dependencies in node_modules
        use: {
          loader: 'babel-loader',
          options: {
            presets: ['@babel/preset-env'] // Use preset-env for transpiling
          }
        }
      },
      {
        test: /\.css$/,
        use: ['style-loader', 'css-loader']
      },
      {
        test: /\.html$/,
        use: ['html-loader']
      },
      {
        test: /\.(png|jpe?g|gif|svg)$/i, // Include image file types
        type: 'asset/resource', // Use asset/resource to handle images
        generator: {
          filename: '[name][ext]' // Keep original file names and extensions
        }
      }
    ]
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: './public/index.html',
      filename: 'index.html',
    }),
    new MiniCssExtractPlugin({
      filename: '[name].[contenthash].css'
    }),
  ],
  mode: 'production'
};
