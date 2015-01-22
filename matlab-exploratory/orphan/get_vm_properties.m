% Extracts relevant information from non-noise recordings from 
% STA experiments.
% Inputs:
%   day: day of experiment eg. Feb_01_13
%   fnum: file number eg. 01, 02, etc.
%   trial: RTXI / hdf5 trial number eg. 1, 2, 3, etc.
%   varargin: arguments are passed GetSpikes Ted's spike detector.
%             See GetSpikes for valid options.  Useful for manually
%             spike detection when tweaks to the parameters are 
%             required.
% Outputs:
%   res: structure that contains many properties of the spike train
%       vm: raw voltage in mV
%       svm: smoothed voltage, sometimes nicer for plotting
%       time: useful for plotting
%       spikes: structure of spikes and their properties from GetSpikes
%       inds: indecies of spikes in the voltage trace
%       cv, sph, rate, minv: voltage trace properties
%       bin_center, bin_count: voltage bins, histogram of voltage values

function res = get_vm_properties(day, fnum, trial, varargin)   
    
    % retrieve the trial, get voltage and smooth
    data = getTrial(makefilename(day, fnum, 'STA', 'sta'), trial);
    res.vm = data.data(:,1) * 1e3;
    dt = data.exp_dt * 1000; fs = round(1 / dt) * 1e3;
    res.svm = mne_smooth(res.vm, 80, 3);
    res.time = data.time;
    if (strcmp(day, 'Feb_14_13') && trial == 3) % silly special case
        data.data = data.data(10*5e3+1:end,1);
        res.vm = data.data(:,1) * 1e3;
        res.svm = mne_smooth(res.vm, 80, 3);
        res.time = dt/1000:dt/1000:20;
    end
    
    % Ted's spike detection (do on raw, unfiltered data)
    res.spikes = GetSpikes(dt, res.vm, varargin{:});
    res.inds = round(res.spikes.times .* fs / 1000);
    % spike rate, use built in rate calculation?
    res.rate = length(res.inds) ./ data.length;

    % cv
    if (length(res.inds) > 1)  % need at least 2 spikes for CV
        isis = diff(res.inds / round(1 / data.exp_dt));
        res.cv = std(isis) / mean(isis);
    else
        res.cv = 0;
    end
    
    % histgrams of voltage
    vlim = [-70 -30]; nbins = 60;
    res.bin_center = linspace(vlim(1), vlim(2), nbins);
    res.bin_count = histc(res.vm, res.bin_center);

    % spike burst voltage plateau
    if (~isempty(res.inds))
        res.minv = mean(res.spikes.postMinV.v);
    else
        res.minv = mean(res.vm);
    end
    
    % spike height
    if (~isempty(res.inds))
        res.sph = mean(res.spikes.maxV.v - res.spikes.postMinV.v);
    else
        res.sph = 0;
    end