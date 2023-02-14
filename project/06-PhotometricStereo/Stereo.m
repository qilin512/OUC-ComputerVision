function [z] = Stereo( directory, imagename, numImages)

%   ***********************************************************************
%   Read the lights and directions:
%   ***********************************************************************
    lightfile = strcat( directory, '/', 'lights2.txt')
    fid = fopen(lightfile, 'r');
    numLights = 1;
    numLights = fscanf(fid, '%d \n', [1]);

    LightMatrix = [];
    for i = 1:numLights
        lightDir = fscanf(fid, '%f %f %f \n', [3]);
	lightDir = lightDir/norm(lightDir);
	lightMatrix(i,:) = lightDir;
    end

%   *************************************************************************
%   Read the mask file and threshold the values
%   *************************************************************************
    maskfile  = strcat( directory, '/', imagename, '/', imagename,'.mask.tiff')
    maskImage = imread( maskfile );

    nrows  = size(maskImage,1);
    ncols  = size(maskImage,2);

    maxval = max(max(maskImage) );

    for i = 1:nrows
    for j = 1:ncols
       if( maskImage(i,j) == maxval)
           maskImage(i,j) = 1;
       else
           maskImage(i,j) = 0;
       end
    end
    end

%   *****************************************************************************
%   Read all the images .,, ( In RGB Format...
%   *****************************************************************************
    accumImage = zeros(nrows, ncols, 3);
%   Read all the images..
    for im = 1:numImages
        id = num2str(im-1);
        filename = strcat( directory, '/', imagename, '/', imagename, '.', id, '.tiff');
	newImage = imread(filename);
	if( size(newImage,1) ~= nrows) 
	    fprintf( ' mask image and source image size do not match ');
	    return;
        end
	if( size(newImage,2) ~= ncols) 
	    fprintf( ' mask image and source image size do not match ');
	    return;
        end

        for i = 1:nrows
        for j = 1:ncols
            accumImage(i,j,1) = accumImage(i,j,1) + double(newImage(i,j,1));
            accumImage(i,j,2) = accumImage(i,j,2) + double(newImage(i,j,2));
            accumImage(i,j,3) = accumImage(i,j,3) + double(newImage(i,j,3));
        end
        end

	images(:,:,:,im) = newImage;
	grayImageSet(:,:,im) = rgb2gray(newImage);
    end

    for i = 1:nrows
    for j = 1:ncols
        r = accumImage(i,j,1);
        g = accumImage(i,j,2);
	b = accumImage(i,j,3);
       if( r  < 5.0 || g < 5.0 || b < 5.0 )
           maskImage(i,j) = 0;
       end
    end
    end

%   *****************************************************************************
%   Photometric Calculations starts from here 
%   *****************************************************************************
    z = zeros(nrows, ncols);

%   *****************************************************************************
%   Process Red Channel for Red Albedo
%   *****************************************************************************
   [surfNormals, albedo] = NormalMap(images, lightMatrix, maskImage, 1);
   save_data( surfNormals, albedo, maskImage, z, 'redChannel.dat');

%   *****************************************************************************
%   Process Green Channel for Green Albedo
%   *****************************************************************************
   [surfNormals, albedo] = NormalMap(images, lightMatrix, maskImage, 2);
   save_data( surfNormals, albedo, maskImage, z, 'greenChannel.dat');

%   *****************************************************************************
%   Process Blue Channel for Blue Albedo
%   *****************************************************************************
   [surfNormals, albedo] = NormalMap(images, lightMatrix, maskImage, 3);
   save_data( surfNormals, albedo, maskImage, z, 'blueChannel.dat');

%   *****************************************************************************
%   Use Gray channel for Normal and Depth Map
%   *****************************************************************************

   [surfNormals, albedo] = NormalMap(images, lightMatrix, maskImage, 0);
   z = DepthMap( surfNormals, maskImage );
   save_data( surfNormals, albedo, maskImage, z, 'grayChannel.dat');

   surfl(z); shading interp; colormap gray

end

%*******************************************************************************
function save_data( surfNormals, albedo, maskImage, z, filename )

   fid = fopen( filename, 'w' );

   nrows  = size(maskImage,1);
   ncols  = size(maskImage,2);

   for i = 1:nrows
   for j = 1:ncols
       xg(i,j) = double(j)/double(ncols);
       yg(i,j) = double(nrows-i+1)/double(nrows);
   end
   end

   fprintf( fid, '%d %d \n', nrows, ncols); 
   for i = 1:nrows
   for j = 1:ncols
       nx  = surfNormals(i,j,1);
       ny  = surfNormals(i,j,2);
       nz  = surfNormals(i,j,3);
       alb = albedo(i,j);
       if( maskImage(i,j)  )
           msk = 1;
       else
           msk = 0;
       end
       fprintf( fid, '%d %f %f %f %f %f %f %f \n', msk, xg(i,j), yg(i,j), z(i,j), nx, ny, nz, alb);
   end
   end

end

%*******************************************************************************

function id = getPixelIndex(i, j, nrows, ncols)
    id = (i-1)*ncols + j;
end

%*******************************************************************************
