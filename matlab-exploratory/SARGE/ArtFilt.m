function outVect = ArtFilt(inVect,samp,HPass,isBandpass)

nPoles=4;
if (~exist('HPass','var'))
    HPass=200;
end

if isBandpass == 0 % Only high pass filter
    Wn=HPass/(samp/2);
    [b,a]=butter(nPoles, Wn, 'high');
else % Band pass filter
    Wn=[HPass 5000]/(samp/2);
    [b,a]=butter(nPoles, Wn);
end

outVect=filtfilt(b,a,inVect);