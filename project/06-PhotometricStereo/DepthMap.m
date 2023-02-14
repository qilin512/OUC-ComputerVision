function [z] = DepthMap(surfNormals, maskImage )

   z = [];

   nrows = size(maskImage, 1)
   ncols = size(maskImage, 2)

   [objectPixelRow objectPixelCol] = find(maskImage);
   objectPixels = [objectPixelRow objectPixelCol]; 

% Create an index matrix to quickly retrieve the index number of the pixel.
% index: A matrix that records the index of the pixel.

  index = zeros(nrows, ncols);

% Total number of Pixels within the mask;
  numPixels =  size(objectPixels, 1);

  for d = 1:numPixels
      pRow = objectPixels(d, 1);
      pCol = objectPixels(d, 2);
      index(pRow, pCol) = d;
  end

% M: A (2*# pixel) by (# pixel) matrix that is used to calculate Z.
% N: A (2*# pixel) by 1 matrix which records the normal vector ratio of Z.
% indexNum: The index number of the pixel that is being examed.

  M = sparse(2*numPixels, numPixels);
  b = zeros(2*numPixels, 1);

  for d = 1: numPixels
      pRow = objectPixels(d, 1);
      pCol = objectPixels(d, 2);
      nx = surfNormals(pRow, pCol, 1);
      ny = surfNormals(pRow, pCol, 2);
      nz = surfNormals(pRow, pCol, 3);      

      if (index(pRow, pCol+1) > 0) && (index(pRow-1, pCol) > 0)     
          % Both its (X+1, Y) and (X, Y+1) are still inside the object.
          M(2*d-1, index(pRow, pCol))   = 1;
          M(2*d-1, index(pRow, pCol+1)) = -1;   % (X+1, Y)
          b(2*d-1, 1) = nx / nz;   

          M(2*d, index(pRow, pCol))     = 1;
          M(2*d, index(pRow-1, pCol))   = -1;     % (X, Y+1)
          b(2*d, 1) = ny / nz;  

      elseif (index(pRow-1, pCol) > 0)     
          % Its (X, Y+1) is still inside the object, but (X+1, Y) is outside the object.
          f = -1;
          if (index(pRow, pCol+f) > 0)
              M(2*d-1, index(pRow, pCol)) = 1;
              M(2*d-1, index(pRow, pCol+f)) = -1;   % (X+f, Y)
              b(2*d-1, 1) = f * nx / nz;    
          end
          M(2*d, index(pRow, pCol)) = 1;
          M(2*d, index(pRow-1, pCol)) = -1;     % (X, Y+1)
          b(2*d, 1) = ny / nz;      

      elseif (index(pRow, pCol+1) > 0)     
          % Its (X+1, Y) is still inside the object, but (X, Y+1) is outside the object.
          f = -1;
          if (index(pRow-f, pCol) > 0)
              M(2*d, index(pRow, pCol)) = 1;
              M(2*d, index(pRow-f, pCol)) = -1;     % (X, Y+f)
              N(2*d, 1) = f*ny / nz;               
          end
          M(2*d-1, index(pRow, pCol)) = 1;
          M(2*d-1, index(pRow, pCol+1)) = -1;   % (X+1, Y)
          N(2*d-1, 1) = nx / nz; 

      else     % Both its (X+1) and (Y+1) are outside the object.
          f = -1;
          if (index(pRow, pCol+f) > 0)
              M(2*d-1, index(pRow, pCol)) = 1;
              M(2*d-1, index(pRow, pCol+f)) = -1;   % (X+f, Y)
              N(2*d-1, 1) = f * nx / nz;           
          end
          f = -1;
          if (index(pRow-f, pCol) > 0)
              M(2*d, index(pRow, pCol)) = 1;
              M(2*d, index(pRow-f, pCol)) = -1;     % (X, Y+f)
              b(2*d, 1) = f*ny / nz;              
          end
      end
  end

  x = M \ b;
  x = x - min(x);

  tempShape = zeros(nrows, ncols);
  for d = 1:numPixels
      pRow = objectPixels(d, 1);
      pCol = objectPixels(d, 2);
      tempShape(pRow, pCol) = x(d, 1);
  end

  z  = zeros( nrows, ncols);
  for i = 1:nrows
      for j = 1:ncols
          z(i, j) = tempShape(nrows-i+1, j);
      end
  end

end

%*******************************************************************************

function id = getPixelIndex(i, j, nrows, ncols)
    id = (i-1)*ncols + j;
end

%*******************************************************************************
