
import numpy as np
import FastWavelet

window = np.hamming(256)
FWL = FastWavelet.FastWavelet(0.5, window)
input = np.array([0.23761270765220133,
0.095859583706103257,
0.8106589199891846,
0.33830606524643925,
0.37457330625407192,
0.63259538116809355,
0.11182216439601578,
0.43479695956179198,
0.67839770473850125,
0.39587282357155129,
0.23049385731410676,
0.1190269348557188,
0.22395826589601353,
0.88634792426200948,
0.39190139252368428,
0.12282163967370874,
0.19066516194249417,
0.3581531949725475,
0.79151883868160877,
0.69030191236535987,
0.31304010532567439,
0.081582924196854645,
0.84873034377898504,
0.70570535504523058,
0.66222152453675542,
0.49617068445069923,
0.17325148020279013,
0.42618692155548576,
0.96881682576825112,
0.16028075539268982,
0.13497608214628987,
0.54498717421306242,
0.47572406298267111,
0.98664961723437816,
0.58932783762993179,
0.40279256373765981,
0.60809419096155304,
0.7493942278898833,
0.04278163839566651,
0.58565695696919817,
0.21704210359061438,
0.58754115569599219,
0.74906737548896229,
0.41401098919485912,
0.9876659719037516,
0.74453782811420044,
0.070786157062153632,
0.23602755831206101,
0.71757680867415563,
0.91028159331390734,
0.6047461770605026,
0.032533308085675339,
0.37091621564110189,
0.28381193480298372,
0.99432768482705347,
0.30595887736992289,
0.79699482849771719,
0.51971592500354813,
0.70383197852057788,
0.026641459098485587,
0.10322883657013515,
0.28270209075555019,
0.32237670091636039,
0.91832596899738128,
0.066024110928171176,
0.55309680531664485,
0.15594457353245117,
0.12670680359248154,
0.58039227481825728,
0.9792436865068519,
0.40073716783213431,
0.83787377914339201,
0.90265788673192726,
0.030634361194810111,
0.2588440966568536,
0.37243761569772915,
0.32752177267076032,
0.17217081369408371,
0.13220563687073705,
0.8974524903017812,
0.23849580187515007,
0.37086772320463324,
0.57330382530454649,
0.58748919504758612,
0.16384877418503874,
0.8693164871086011,
0.27444644365175486,
0.56803765151977237,
0.28925571411952611,
0.36912611595427958,
0.41907316615782819,
0.8494224516007185,
0.69354923361276433,
0.32417748871135665,
0.59898608371250128,
0.78918990650995569,
0.2654718524208991,
0.23529120574777307,
0.56867319601851363,
0.98143272638548873,
0.64115577252355216,
0.010302576615903392,
0.96572177849422247,
0.058118276819344117,
0.66356493546280759,
0.73179821087201513,
0.60400632649976183,
0.66066185090502383,
0.93216988799990941,
0.74131422699173233,
0.56951335881662135,
0.4482395670937781,
0.32690952170371435,
0.70678467738979001,
0.033201069716643161,
0.033122531147511625,
0.95119767787216503,
0.24399017297566949,
0.44693978415290236,
0.31173405836304091,
0.69728336036293737,
0.3562092122254763,
0.91823287529855657,
0.79568046775864809,
0.60568251334135648,
0.28949121104589992,
0.74550970531156557,
0.45764627192482132,
0.24827086170650814,
0.6094693499973135,
0.10710783153773606,
0.2645925133962278,
0.5868021273077666,
0.37500248542067038,
0.20121962424293771,
0.12443634596217656,
0.67098294668830971,
0.36876840598568639,
0.30539146585078725,
0.51198679021853877,
0.33614789771731213,
0.62779137939771912,
0.10436725655186063,
0.84607450365658243,
0.98595314709886517,
0.53649648164556885,
0.69031059800555472,
0.93510687364590883,
0.22365084358195886,
0.92697549837108006,
0.5129765535299905,
0.55650471266968216,
0.028713073713381498,
0.78701994355325045,
0.14303913349415676,
0.33965207764087513,
0.2516051242863202,
0.34471266653655674,
0.64712546186479336,
0.91943685982715273,
0.90164888150971723,
0.21882413355255548,
0.086370600005431286,
0.082963559910967488,
0.67071393210586694,
0.11763048564005985,
0.16616711415105934,
0.060661684137512051,
0.94230692614830924,
0.5293586558277289,
0.82623744586708492,
0.67416048544208418,
0.90399093738709413,
0.39696501030326925,
0.3143997652326086,
0.87744666192582088,
0.0019357561255414213,
0.69653754477501828,
0.80254701040144805,
0.66555888790324325,
0.23884633369193164,
0.1292918473746546,
0.98319445206062739,
0.029904016271518508,
0.34824276989215652,
0.67775524283666977,
0.12055628150897957,
0.31359869486300862,
0.53733349325587498,
0.44033497339372374,
0.95235914549081824,
0.81818193864923394,
0.70401451933082948,
0.97488356383138552,
0.78658165934077817,
0.056979042676516101,
0.77724561386332447,
0.8425339377545078,
0.27887712857461389,
0.36534624293486406,
0.97860935082572997,
0.49474225291395624,
0.96630832000503997,
0.62676234959526544,
0.2243832347028174,
0.64652910641746952,
0.53908316793282052,
0.72679011203691646,
0.21048147537715467,
0.9341306235017478,
0.56407433107394744,
0.56883193608618865,
0.87694513215476078,
0.86587290223913571,
0.77937612785099541,
0.80127667315200624,
0.28821015270328221,
0.66198500758673939,
0.35432554157630236,
0.99408775973967023,
0.51281464244458153,
0.2999809876627737,
0.55545982577832975,
0.37210600413214068,
0.42655728715861785,
0.54405974200500529,
0.0032055376748668651,
0.3861865543918781,
0.19098921407264424,
0.12142893149153267,
0.43578009658172034,
0.86732827151652192,
0.24012894011783648,
0.32616366531326335,
0.24256941183146208,
0.93752339298132148,
0.17400293106286702,
0.61910635805216208,
0.090040611312998009,
0.86463817015556932,
0.90896926176780812,
0.32544135581239053,
0.30434990315957156,
0.54371304699141054,
0.73695396957011461,
0.81148291841824338,
0.18387029519363174,
0.93506282557841047,
0.13003715051367293,
0.13246876564477472,
0.13756746926191876,
0.52732133957977423,
0.31523855365857811,
0.71403323587290002,
0.54889438412821556,
0.62184973870435356,
0.8205002388654089,
0.56031312225162022,
0.13779585842596587,
0.76874400843603918,
0.83372479494259855,
0.90360889161611202,
0.24989819349190656,
0.9119743301222567,
0.052421066491635471,
0.87560344367883458,
0.27922888583154382,
0.77404440185414147,
0.04070608513200269,
0.85846795952907828,
0.48416535156383222,
0.96112422180161039,
0.39148394410377485,
0.22053986645937085,
0.98878499963644872,
0.83863840206612361,
0.2058865255304847,
0.83892238911433969,
0.19750487423529528,
0.12180298408600021,
0.079512009569069897,
0.96172754491886525,
0.39055744509940393,
0.96188760878454715,
0.84571859269541727,
0.59261787443179403,
0.3587259636782294,
0.082356067797794252,
0.74603245528771878,
0.48614110990657233,
0.33142193906815764,
0.79947643742922414,
0.18919082653501007,
0.28371851624393141,
0.31172882264975632,
0.85541893434350957,
0.46818093846812237,
0.1899403258454565,
0.99089079052515594,
0.45001224249150606])
answer = np.sum(window*input[0:256])
result = FWL.PushSamples(input)
print "The Answer Is: " + repr(answer)
print "We Got: " + repr(result)

print " "

# plt.figure()
# plt.plot(np.abs(output))
# plt.figure()
# plt.plot(np.abs(the_ans[0:2056]))