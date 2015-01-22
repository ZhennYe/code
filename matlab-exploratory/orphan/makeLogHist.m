function makeLogHist()
    ph = get(gca,'children');
    vn = get(ph,'Vertices'); 
    vn(:,2) = vn(:,2) + 1;
    set(ph,'Vertices',vn)
    set(gca,'YScale','log');