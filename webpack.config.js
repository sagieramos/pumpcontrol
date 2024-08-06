import path from 'path';
import HtmlWebpackPlugin from 'html-webpack-plugin';
import MiniCssExtractPlugin from 'mini-css-extract-plugin';
import CompressionPlugin from 'compression-webpack-plugin';
import { fileURLToPath } from 'url';

// Resolve __dirname in ES module scope
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

//const subDir = 'login';
const subDir = 'dashboard';

export default {
  entry: `./public/${subDir}/_${subDir}.js`,
  output: {
    filename: `_${subDir.charAt(0)}bundle.js`,
    path: path.resolve(__dirname, 'data'),
    assetModuleFilename: '[name][ext]'
  },
  module: {
    rules: [
      {
        test: /\.js$/,
        exclude: /node_modules/,
        use: {
          loader: 'babel-loader',
          options: {
            presets: ['@babel/preset-env']
          }
        }
      },
      {
        test: /\.css$/,
        use: ['style-loader', 'css-loader']
        //use: [MiniCssExtractPlugin.loader, 'css-loader']
      },
      {
        test: /\.html$/,
        use: ['html-loader']
      },
      {
        test: /\.(png|jpe?g|gif|svg)$/i,
        type: 'asset/resource',
        generator: {
          filename: '[name][ext]'
        }
      }
    ]
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: `./public/${subDir}/_${subDir}.html`,
      filename: `_${subDir}.html`
    }),
    new MiniCssExtractPlugin({
      filename: `_${subDir}.[contenthash].css`
    }),
    new CompressionPlugin({
      filename: '[path][base].gz',
      algorithm: 'gzip',
      test: /\.(js|css|html|svg|png|jpe?g|gif)$/,
      threshold: 5120,
      minRatio: 0.8
    }),
  ],
  mode: 'production'
};
