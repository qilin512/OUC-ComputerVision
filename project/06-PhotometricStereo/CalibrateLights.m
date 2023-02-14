function [L] = CalibrateLight(directory, numLights)
% This function is used to calibrate the lighting direction.

% Find all the files in that directory.
  if directory (end) ~= '/'
      fileName = [directory '/'];
  end

  fileName = [fileName 'chrome.'];
% Calculate the center of the chrome ball.
  maskFileName = [fileName 'mask.tiff'];
  circle = imread(maskFileName);
%  circle = rgb2gray(circle);

% Calculate the center of the chrome ball.
  maxval = max( max( circle ) );
  [circleRow circleCol] = find(circle == maxval);
  maxRow = max(circleRow);
  minRow = min(circleRow);
  maxCol = max(circleCol);
  minCol = min(circleCol);
  xc     = double((maxCol + minCol)/2);
  yc     = double((maxRow + minRow)/2);
  center = [xc, yc]
  radius = double((maxRow - minRow)/2)

% R: The reflection direction.
  R = [0 0 1.0];
  L = [];

% Calculate the lighting direction.
  for i = 1:numLights
      imgFileName = [fileName num2str(i-1) '.tiff'];
      image = imread(imgFileName);
      image = rgb2gray(image);
      maxval = max( max( image ) );
      [pointRow, pointCol] = find(image == maxval);
      nSize  = size( pointRow, 1);
      px     = sum(pointCol)/double(nSize);
      py     = sum(pointRow)/double(nSize);
      Nx     =   px - xc;
      Ny     = -(py - yc);
      Nz     = sqrt( radius^2 - Nx^2 - Ny^2 );
      normal = [Nx, Ny, Nz];
      normal = normal/radius;
      NR     = normal(1)*R(1) + normal(2)*R(2) + normal(3)*R(3);
      L(i,:) = 2*NR*normal - R;
  end

% Write the new lighting direction into a test file.
  fid = fopen('calibratedLight.txt', 'w');
  fprintf( fid, '%d \n', numLights);
  for row = 1:numLights
      fprintf(fid, ' %10.5f %10.5f %10.5f \n', L(row,1), L(row,2), L(row,3) );
  end
  fclose(fid);
end

