#include "video_encoder.hpp"

int main() {

    createDirIfNotExists("./configurazioni");


	char input_action = ' ';

	while (true) {

		input_action = showMenu();


		if (input_action == 'q' || input_action == 'Q') return 0;
		
		switch(input_action){
		    case 'q':
		    case 'Q':
		        return 0;
		    case '0':
                cout << endl << endl;
                cout << printLine(110) << endl;
                cout << format("LISTA CONFIGURAZIONI", 110) << endl;
                cout << printLine(110) << endl;
		        mostraConfigurazioni();
		        break;
		    case '1':
                cout << endl;
		        creaConfigurazione();
		        break;
            case '2':
                cout << endl;
                eliminaConfigurazione();
                break;
            case '3':
                cout << endl;
                eseguiCompressione();
                break;
            case '4':
                cout << endl;
                fullCompression();
                break;
            case '5':
                cout << endl;
                mostraRisultato();
                break;
            case '6':
                cout << endl;
                mostraRisultati();
                break;
            case '7':
                cout << endl;
                mostraRisultatiMedi();
                break;
            case '8':
                cout << endl;
                mostraRisultatiConfigurazione();
                break;
            case '9':
                cout << endl;
                clearAll();
                break;
		    default:
		        break;

		}
	}

	return 0;
}

string askClip(){


    string clip;
    int val = 0;

    while(val < 1 || val > 5){
        cout << endl << endl;
        cout << printLine(110) << endl;
        cout << format("SELEZIONA LA CLIP", 110) << endl;
        cout << printLine(110) << endl;
        cout << formatLeft(" 1) Akiyo", 110) << endl;
        cout << formatLeft(" 2) Coastguard", 110) << endl;
        cout << formatLeft(" 3) Container", 110) << endl;
        cout << formatLeft(" 4) Hall", 110) << endl;
        cout << formatLeft(" 5) News", 110) << endl;
        cout << printLine(110) << endl;

        cin >> val;
        switch(val){
            case 1:
                clip = "Akiyo";
                break;
            case 2:
                clip = "Coastguard";
                break;
            case 3:
                clip = "Container";
                break;
            case 4:
                clip = "Hall";
                break;
            case 5:
                clip = "News";
                break;
            default:
                cout << "Valore non valido!" << endl;
                break;
        }
    }

    return clip;
}

string askCodec(){
    string codec;
    int val = 0;

    while(val < 1 || val > 3){
        cout << endl << endl;
        cout << printLine(110) << endl;
        cout << format("SELEZIONA IL CODEC", 110) << endl;
        cout << printLine(110) << endl;
        cout << formatLeft(" 1) mpeg2video (MPEG-2)", 110) << endl;
        cout << formatLeft(" 2) libvpx-vp9 (VP9)", 110) << endl;
        cout << formatLeft(" 3) libx264 (H.264)", 110) << endl;
        cout << printLine(110) << endl;

        cin >> val;

        switch(val){
            case 1:
                codec = "mpeg2video";
                break;
            case 2:
                codec = "libvpx-vp9";
                break;
            case 3:
                codec = "libx264";
                break;
            default:
                cout << "Valore non valido!" << endl;
                break;
        }
    }
    return codec;
}

int askConf(){

    cout << endl << endl;
    cout << printLine(110) << endl;
    cout << format("SELEZIONA LA CONFIGURAZIONE", 110) << endl;
    cout << printLine(110) << endl;
    mostraConfigurazioni();
    int index;
    cin >> index;

    return index;
}

void eseguiCompressione(){
    string clip;
    string codec;
    int conf;

    clip = askClip();

    codec = askCodec();

    conf = askConf();

    encode(clip, codec, conf);
}

void encode(string clip, string codec, int conf){
    struct dirent *entry;
    DIR *dir = opendir("./configurazioni");

    int indice = 1;
    while ((entry = readdir(dir)) != NULL) {
        if( strcmp( entry->d_name, "." ) && strcmp( entry->d_name, ".." ) ){
            if(indice == conf){

                string file = "./configurazioni/";
                file = file + entry->d_name;
                string conf_name = entry->d_name;
                closedir(dir);
                ifstream infile(file);
                string command;
                string psnr_command;
                string ssim_command;
                getline(infile, command);
                getline(infile, psnr_command);
                getline(infile, ssim_command);

                //Composizione del comando per la compressione
                if(!replaceAndCheck(command, "<codec>", codec, 3) ||
                !replaceAndCheck(command, "<clip>", clip, 3) ||
                !replaceAndCheck(psnr_command, "<clip>", clip, 3) ||
                !replaceAndCheck(psnr_command, "<codec>", codec, 2) ||
                !replaceAndCheck(ssim_command, "<clip>", clip, 3) ||
                !replaceAndCheck(ssim_command, "<codec>", codec, 2)
                ){
                    cout << "ERRORE -- Recupero della configurazione fallito!" << endl;
                    return;
                };

                //Eliminazione di eventuali file di output e statistiche se già esiste
                //Se non esistono le cartelle necessarie queste vengono create
                clearDirs(clip, codec, conf_name);

                //Avvio comando ffmpeg
                cout << endl << endl;
                cout << printLine(110) << endl;
                cout << format("AVVIO ENCODING", 110) << endl;
                cout << printLine(110) << endl;
                string str = "Clip: " + clip;
                cout << format(str, 110) << endl;
                str = "Codec: " + codec;
                cout << format(str, 110) << endl;
                str = "Configurazion: " + conf_name;
                cout << format(str, 110) << endl;
                cout << printLine(110)<< endl << endl;


                auto start = std::chrono::high_resolution_clock::now();
                string comando = command + " && " + psnr_command + "&&" +  ssim_command;
                cout << endl << "Eseguo compressione: " << endl << comando << endl << endl;
                system(comando.c_str());

                auto finish = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double> elapsed = finish - start;

                ofstream timeFile;
                string path = "./Clips/" + clip + "/" + codec + "/" + getConfNameByIndex(conf) + "/time.txt";
                timeFile.open (path.c_str());
                timeFile << elapsed.count();
                timeFile.close();

                cout << endl << " -- ENCODING COMPLETATO! -- " << endl;
                return;
            }
            indice++;
        }
    }

    closedir(dir);
    cout << "ERRORE: Configurazione Non valida!" << endl;
}

void clearDirs(string clip, string codec, string conf_name){

    createDirs(clip, codec, conf_name);

    string stat_file_name = "./Clips/" + clip + "/" + codec + "/" + conf_name + "/stats.txt";
    remove(stat_file_name.c_str());

    stat_file_name = "./Clips/" + clip + "/" + codec + "/" + conf_name + "/error.txt";
    remove(stat_file_name.c_str());

    stat_file_name = "./Clips/" + clip + "/" + codec + "/" + conf_name + "/ssim.txt";
    remove(stat_file_name.c_str());

    stat_file_name = "./Clips/" + clip + "/" + codec + "/" + conf_name + "/time.txt";
    remove(stat_file_name.c_str());
}

void clearOutput(string clip){
    struct dirent *entry;
    string path = "./Clips/" + clip;
    DIR *dir = opendir(path.c_str());

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..") && strcmp(entry->d_name, "original.yuv")) {
            string name = path + "/" + entry->d_name;

            if (entry->d_type == DT_DIR) {
                clearFolder(name);
                remove(name.c_str());
            }else{
                remove(name.c_str());
            }
        }
    }

    closedir(dir);
}

void clearFolder(string path){
    struct dirent *entry;
    DIR *dir = opendir(path.c_str());

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            string name = path + "/" + entry->d_name;

            if (entry->d_type == DT_DIR) {
                clearFolder(name);
                remove(name.c_str());
            }else{
                remove(name.c_str());
            }
        }
    }

    closedir(dir);
}

void clearAll(){
    const char *clips[5] = { "Akiyo", "Coastguard", "Container", "Hall", "News" };

    for(int i = 0; i < 5 ; i++){
        cout << "Clear clip " << clips[i] << endl;
        clearOutput(clips[i]);
    }

    cout << "Pulizia completata!" << endl << endl;
}

bool replaceAndCheck(std::string& str, const std::string& from, const std::string& to, int numero){
    bool found = false;
    for(int i = 0; i < numero; i++){
        found = replace(str, from, to);
        if(!found){
            return false;
        }
    }
    //Devono esserci esattamente 'numero' occorrenze del placeholder nella stringa
    if(replace(str, from, to)){
        return false;
    }

    return true;
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

float getPSNRFromLine(std::string& line){
    size_t start_pos = line.find("psnr_avg:");
    if(start_pos == std::string::npos){
        return 0;
    }
    size_t end_pos = line.find("psnr_y:");
    if(start_pos == std::string::npos){
        return 0;
    }

    string val = line.substr(start_pos + 9, end_pos -1);

    return stof(val);

}

float getSSIMFromLine(std::string& line){
    //n:1 Y:0.977143 U:0.981142 V:0.984072 All:0.978965 (16.770485)


    size_t start_pos = line.find("All:");
    if(start_pos == std::string::npos){
        return 0;
    }
    size_t end_pos = line.find("(");
    if(start_pos == std::string::npos){
        return 0;
    }

    string val = line.substr(start_pos + 4, end_pos -1);

    return stof(val);

}


void creaConfigurazione(){
    int value_bf;
    int value_g;
    string bf;
    string g;

    cout << "Definisci una nuova configurazione:" << endl;

    value_g = 0;
    while(value_g < 1){
        cout << "Indica il valore di 'g' (Dimensione Group Of Picture):" << endl;
        cin >> value_g;
    }
    g = " -g " + std::to_string(value_g) + " ";

    value_bf = -1;
    while(value_bf < 0 || value_bf > value_g || value_bf > 16){
        cout << "Indica il valore di 'bf' (MAX 16 - '0' per disabilitare le frames B):" << endl;
        cin >> value_bf;
        if(value_bf < 0 || value_bf > value_g){
            cout << "Valore non valido (deve essere compreso tra 0 e 16 e minore di 'g')" << endl;
        }
    }
    bf = " -bf " + std::to_string(value_bf) + " ";

    string name;
    bool esiste = true;
    while(esiste){
        cout << "Indica il nome della configurazione (evitare gli spazi):" << endl;
        cin >> name;

        esiste = false;
        if(nomeConfigurazioneUsato(name)){
            cout << "ERRORE -- Nome configurazione già in uso!" << endl;
            esiste = true;
        }
    }
    string command = "ffmpeg -r 30 -s cif -i \"./Clips/<clip>/original.yuv\" -c:v <codec> " + bf + g + " -vstats_file \"./Clips/<clip>/<codec>/" + name + "/stats.txt\" -pix_fmt yuv420p -y \"./Clips/<clip>/<codec>/" + name + "/out.mp4\"";
    string psnr_command = "ffmpeg -s cif -i \"./Clips/<clip>/original.yuv\" -i \"./Clips/<clip>/<codec>/" + name + "/out.mp4\" -lavfi  psnr=\"stats_file=./Clips/<clip>/<codec>/" + name + "/error.txt\" -f null -";
    string ssim_command = "ffmpeg -s cif -i \"./Clips/<clip>/original.yuv\" -i \"./Clips/<clip>/<codec>/" + name + "/out.mp4\" -lavfi  ssim=\"stats_file=./Clips/<clip>/<codec>/" + name + "/ssim.txt\" -f null -";

    ofstream outfile ("./configurazioni/" + name);
    outfile << command << endl;
    outfile << psnr_command << endl;
    outfile << ssim_command << endl;
    outfile << value_g << endl;
    outfile << value_bf << endl;
    outfile.close();

    cout << "Configurazione salvata!" << endl;
}

bool nomeConfigurazioneUsato(string name){
    struct dirent *entry;
    DIR *dir = opendir("./configurazioni");

    while ((entry = readdir(dir)) != NULL) {
        if(name == entry->d_name){
            return true;
        }
    }
    closedir(dir);

    return false;
}

void mostraConfigurazioni(){

    struct dirent *entry;
    DIR *dir = opendir("./configurazioni");

    int indice = 1;
    while ((entry = readdir(dir)) != NULL) {
        if( strcmp( entry->d_name, "." ) && strcmp( entry->d_name, ".." ) ){
            string bf;
            string g;
            string file = "./configurazioni/";
            file = file + entry->d_name;
            ifstream infile(file);
            string line = "";
            getline(infile, line); //comando
            getline(infile, line); //comando pnsr
            getline(infile, line); //comando ssim
            getline(infile, g); //valore g
            getline(infile, bf); //valore bf


            string str = "(" + std::to_string(indice) + ") " + entry->d_name;
            cout << format(str, 110) << endl;
            str = "Dimensione Group of Pictures: " + g;
            cout << format(str, 110) << endl;
            str = "Numero Massimo di B-frames consecutive: " + bf;
            cout << format(str, 110) << endl;
            cout << printLine(110) << endl;
            indice++;
        }
    }
    closedir(dir);

    if(indice == 1){
        cout << "Nessuna configurazione presente!" << endl;
    }
    return;
}

char showMenu(){
    string input;

    cout << endl << endl;
    cout << printLine(110) << endl;
    cout << format("BENVENUTO", 110) << endl;
    cout << printLine(110) << endl;
    cout << formatLeft(" Seleziona una azione:" ,110) << endl;
    cout << formatLeft(" 0) Mostra le configurazioni esistenti", 110) << endl;
    cout << formatLeft(" 1) Definisci una nuova configurazione", 110) << endl;
    cout << formatLeft(" 2) Elimina una configurazione", 110) << endl;
    cout << formatLeft(" 3) Effettua la compressione di una delle sequenze non compresse", 110) << endl;
    cout << formatLeft(" 4) Esegui la compressione su tutti i video e con tutte le configurazioni (con tutti i codec)", 110) << endl;
    cout << formatLeft(" 5) Mostra Statistiche (Una combinazione)", 110) << endl;
    cout << formatLeft(" 6) Mostra Statistiche (Tutte le combinazioni)", 110) << endl;
    cout << formatLeft(" 7) Mostra Statistiche (Media su tutte le clip)", 110) << endl;
    cout << formatLeft(" 8) Mostra Statistiche relative ad una configurazione", 110) << endl;
    cout << formatLeft(" 9) Clear (Elimina tutti gli output presenti)", 110) << endl;
    cout << formatLeft(" Q) Esci", 110) << endl;
    cout << printLine(110) << endl << endl;

    cin >> input;
    return input[0];
}

void eliminaConfigurazione(){

    cout << endl << endl;
    cout << printLine(110) << endl;
    cout << format("INDICA LA CONFIGURAZIONE CHE VUOI ELIMINARE", 110) << endl;
    cout << printLine(110) << endl;
    mostraConfigurazioni();

    if(getConfNumber() == 0){
        return;
    }

    int index;
    cin >> index;


    struct dirent *entry;
    DIR *dir = opendir("./configurazioni");

    int indice = 1;
    while ((entry = readdir(dir)) != NULL) {
        if( strcmp( entry->d_name, "." ) && strcmp( entry->d_name, ".." ) ){
            if(indice == index){
                string file = "./configurazioni/";
                file = file + entry->d_name;
                closedir(dir);
                if (remove(file.c_str()) == 0)
                    cout << "Configurazione Eliminata!" << endl;
                else
                    cout << "ERRORE -- Eliminazione non riuscita!" << endl;
                return;
            }
            indice++;
        }
    }

    closedir(dir);
    cout << "ERRORE: Indice Non valido!" << endl;
    eliminaConfigurazione();
}

void mostraRisultati(){
    const char *clips[5] = { "Akiyo", "Coastguard", "Container", "Hall", "News" };
    const char *codecs[3] = { "mpeg2video", "libvpx-vp9", "libx264"};
    int confNum = getConfNumber();

    for (int i = 0; i < 5; i++){ //Per ogni clip
        for(int j = 0; j < 3; j++){ //Per ogni codec
            for(int k = 1; k <= confNum; k++){ //Per ogni configurazione

                Stat stat = getStats(clips[i], codecs[j], getConfNameByIndex(k));

                printStat(stat);
            }
        }
    }
    return;
}

void mostraRisultato(){
    cout << endl << endl << endl;

    string clip = askClip();
    string codec = askCodec();
    int conf = askConf();

    cout << endl << endl << endl;

    Stat stat = getStats(clip, codec, getConfNameByIndex(conf));

    printStat(stat);
    return;
}

void printStat(Stat stat){

    cout << printLine(110) << endl;
    cout << format("RISULTATI ENCODING", 110)<< endl;
    cout << printLine(110) << endl;

    string str;
    str = "Clip: ";
    str = str + stat.clip;
    cout << format(str, 110) << endl;
    str = "Codec: ";
    str = str + stat.codec;
    cout << format(str, 110) << endl;
    str = "Configurazione: ";
    str = str + stat.conf;
    cout << format(str, 110) << endl;

    cout << printLine(110) << endl;

    if(stat.error){
        cout << format("----- Nessuna statistica presente! -----", 110) << endl;
    }else{
        str = "PSNR medio su " + printRound(stat.count,0) + " frames: " + printRound(stat.psnr,2);
        cout << format(str, 110) << endl;
        str = "SSIM medio su " + printRound(stat.count,0) + " frames: " + printRound(stat.ssim,2);
        cout << format(str, 110) << endl;
        str = "Dimensione Originale: " + printRound(stat.dimOriginale,2) + " KB";
        cout << format(str, 110) << endl;
        str = "Dimensione Compressa: " + printRound(stat.dimCompressa,2) + " KB";
        cout << format(str, 110) << endl;

        str = "Frames I:" + printRound(stat.count_i,0) + " (" + printRound((stat.count_i*100)/stat.count,2) + "%) - Frames P:" + printRound(stat.count_p,0) + " (" + printRound((stat.count_p*100)/stat.count,2) + "%) - Frames B:" + printRound(stat.count_b,0) + " (" + printRound((stat.count_b*100)/stat.count,2) + "%)";
        cout << format(str, 110) << endl;


        str = "Rapporto di compressione: " + printRound(stat.rapporto,2);
        cout << format(str, 110) << endl;

        str = "Tempo di compressione: " + printRound(stat.time,2) + " secondi";
        cout << format(str, 110) << endl;
    }
    cout << printLine(110) << endl << endl << endl << endl;
}

void mostraRisultatiMedi(){ //Media dei risultati tra le clip
    int confNum = getConfNumber();

    cout << printLine(208) << endl;
    cout << format("CONFIGURAZIONE", 28) << format("MPEG2", 60)<< format("VP9", 60) << format("H.264", 60)<< endl;
    cout << printLine(208) << endl;

    for(int k = 0; k < confNum; k++){ //Per ogni configurazione
        string conf = getConfNameByIndex(k+1);
        Stat statMpeg2 = getMediumStats("mpeg2video", conf);
        Stat statVp9 = getMediumStats("libvpx-vp9", conf);
        Stat statH264 = getMediumStats("libx264", conf);


        printRigaStatisticheMultiple(conf, statMpeg2,statVp9,statH264, true);
    }

    return;
}

void mostraRisultatiConfigurazione(){
    cout << endl;

    int conf = askConf();
    int confNum = getConfNumber();

    if(conf > confNum || conf < 1){
        cout << "ERRORE -- Configurazione non valida" << endl << endl;
        return;
    }

    string confName = getConfNameByIndex(conf);

    cout << printLine(208) << endl;
    cout << format("Clip", 28) << format("MPEG2", 60)<< format("VP9", 60) << format("H.264", 60)<< endl;
    cout << printLine(208) << endl;

    const char *clips[5] = { "Akiyo", "Coastguard", "Container", "Hall", "News" };
    for(int k = 0; k < 5; k++){ //Per ogni clip
        Stat statMpeg2 = getStats(clips[k], "mpeg2video", confName);
        Stat statVp9 = getStats(clips[k], "libvpx-vp9", confName);
        Stat statH264 = getStats(clips[k], "libx264", confName);

        string label = clips[k];
        label = label + " (" + printRound(statMpeg2.dimOriginale,2) + " KB)";

        printRigaStatisticheMultiple(label, statMpeg2,statVp9,statH264, false);
    }

    return;
}

void printRigaStatisticheMultiple(string label, Stat stat1, Stat stat2, Stat stat3, bool media){
    string str;
    string med = "";

    if(media){
        med = "(media) ";
    }

    cout << format("", 28);
    if(!stat1.error){
        str = "Rapporto di compressione " + med + ": " + printRound(stat1.rapporto,2);
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat2.error){
        str = "Rapporto di compressione " + med + ": " + printRound(stat2.rapporto,2);
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat3.error){
        str = "Rapporto di compressione " + med + ": " + printRound(stat3.rapporto,2);
    }else{
        str = "";
    }
    cout << format(str,60);
    cout << endl;


    cout << format("", 28);
    if(!stat1.error){
        str = "Tempo di compressione " + med + ": " + printRound(stat1.time,2) + " secondi";
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat2.error){
        str = "Tempo di compressione " + med + ": " + printRound(stat2.time,2) + " secondi";
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat3.error){
        str = "Tempo di compressione " + med + ": " + printRound(stat3.time,2) + " secondi";
    }else{
        str = "";
    }
    cout << format(str,60);
    cout << endl;


    if(!media){
        cout << format("", 28);
        if(!stat1.error){
            str = "Dimensione Compressa :" + printRound(stat1.dimCompressa,2) + " KB";
        }else{
            str = "";
        }
        cout << format(str,60);
        if(!stat2.error){
            str = "Dimensione Compressa :" + printRound(stat2.dimCompressa,2) + " KB";
        }else{
            str = "";
        }
        cout << format(str,60);
        if(!stat3.error){
            str = "Dimensione Compressa :" + printRound(stat3.dimCompressa,2) + " KB";
        }else{
            str = "";
        }
        cout << format(str,60);
        cout << endl;
    }





    cout << format(label, 28);

    if(!stat1.error){
        str = "PSNR medio su " + printRound(stat1.count,0) + " frames: " + printRound(stat1.psnr,2);
    }else{
        str = "----- Statistica non disponibile su tutte le clip -----";
    }
    cout << format(str,60);
    if(!stat2.error){
        str = "PSNR medio su " + printRound(stat2.count,0) + " frames: " + printRound(stat2.psnr,2);
    }else{
        str = "----- Statistica non disponibile su tutte le clip -----";
    }
    cout << format(str,60);
    if(!stat3.error){
        str = "PSNR medio su " + printRound(stat3.count,0) + " frames: " + printRound(stat3.psnr,2);
    }else{
        str = "----- Statistica non disponibile su tutte le clip -----";
    }
    cout << format(str,60);
    cout << endl;


    cout << format("", 28);
    if(!stat1.error){
        str = "SSIM medio su " + printRound(stat1.count,0) + " frames: " + printRound(stat1.ssim,2);
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat2.error){
        str = "SSIM medio su " + printRound(stat2.count,0) + " frames: " + printRound(stat2.ssim,2);
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat3.error){
        str = "SSIM medio su " + printRound(stat3.count,0) + " frames: " + printRound(stat3.ssim,2);
    }else{
        str = "";
    }
    cout << format(str,60);
    cout << endl;

    cout << format("", 28);
    if(!stat1.error){
        str = "Frames I:" + printRound(stat1.count_i,0) + " (" + printRound((stat1.count_i*100)/stat1.count,2) + "%)";
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat2.error){
        str = "Frames I:" + printRound(stat2.count_i,0) + " (" + printRound((stat2.count_i*100)/stat2.count,2) + "%)";
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat3.error){
        str = "Frames I:" + printRound(stat3.count_i,0) + " (" + printRound((stat3.count_i*100)/stat3.count,2) + "%)";
    }else{
        str = "";
    }
    cout << format(str,60);
    cout << endl;


    cout << format("", 28);
    if(!stat1.error){
        str = "Frames P:" + printRound(stat1.count_p,0) + " (" + printRound((stat1.count_p*100)/stat1.count,2) + "%)";
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat2.error){
        str = "Frames P:" + printRound(stat2.count_p,0) + " (" + printRound((stat2.count_p*100)/stat2.count,2) + "%)";
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat3.error){
        str = "Frames P:" + printRound(stat3.count_p,0) + " (" + printRound((stat3.count_p*100)/stat3.count,2) + "%)";
    }else{
        str = "";
    }
    cout << format(str,60);
    cout << endl;

    cout << format("", 28);
    if(!stat1.error){
        str = "Frames B:" + printRound(stat1.count_b,0) + " (" + printRound((stat1.count_b*100)/stat1.count,2) + "%)";
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat2.error){
        str = "Frames B:" + printRound(stat2.count_b,0) + " (" + printRound((stat2.count_b*100)/stat2.count,2) + "%)";
    }else{
        str = "";
    }
    cout << format(str,60);
    if(!stat3.error){
        str = "Frames B:" + printRound(stat3.count_b,0) + " (" + printRound((stat3.count_b*100)/stat3.count,2) + "%)";
    }else{
        str = "";
    }
    cout << format(str,60);
    cout << endl;
    cout << printLine(208) << endl;
}

Stat getMediumStats(string codec, string conf){
    const char *clips[5] = { "Akiyo", "Coastguard", "Container", "Hall", "News" };
    Stat ret;

    ret.clip = "";
    ret.codec = codec;
    ret.conf = conf;
    ret.error = false;

    ret.count = 0;
    ret.count_i = 0;
    ret.count_p = 0;
    ret.count_b = 0;
    ret.dimOriginale = 0;
    ret.dimCompressa = 0;
    ret.rapporto = 0;
    ret.psnr = 0;
    ret.time = 0;
    ret.ssim = 0;

    for(int i = 0; i < 5; i++){
        Stat stat = getStats(clips[i], codec, conf);

        ret.error = ret.error || stat.error;
        ret.time = ret.time + stat.time;
        ret.psnr = ret.psnr + stat.psnr;
        ret.ssim = ret.ssim + stat.ssim;
        ret.count = ret.count + stat.count;
        ret.count_i = ret.count_i + stat.count_i;
        ret.count_p = ret.count_p + stat.count_p;
        ret.count_b = ret.count_b + stat.count_b;
        ret.rapporto = ret.rapporto + stat.rapporto;

    }

    ret.time = ret.time/5;
    ret.psnr = ret.psnr/5;
    ret.ssim = ret.ssim/5;
    ret.rapporto = ret.rapporto/5;

    return ret;
}

Stat getStats(string clip, string codec, string conf){

    Stat ret;
    ret.error = true;

    ret.clip = clip;
    ret.codec = codec;
    ret.conf = conf;

    createDirs(clip,codec,conf);

    struct dirent *entry;
    string path = "./Clips/" + clip + "/" + codec + "/" + conf;
    DIR *dir = opendir(path.c_str());


    float psnr = 0;
    float ssim = 0;
    float count = 0;
    float time = 0;

    float count_i = 0;
    float count_p = 0;
    float count_b = 0;


    bool foundError = false;
    bool foundStats = false;
    bool foundSSIM = false;
    bool foundTime = false;
    while ((entry = readdir(dir)) != NULL) {
        if( strcmp( entry->d_name, "error.txt" ) == 0){
            foundError = true;
            string file = "./Clips/" + clip + "/" + codec + "/" + conf + "/";
            file = file + entry->d_name;
            ifstream infile(file);
            string line;
            while(getline(infile, line)){
                psnr += getPSNRFromLine(line);
                count += 1;
            }

            psnr = psnr/count;
        }

        if( strcmp( entry->d_name, "ssim.txt" ) == 0){
            float num;
            foundSSIM = true;
            string file = "./Clips/" + clip + "/" + codec + "/" + conf + "/";
            file = file + entry->d_name;
            ifstream infile(file);
            string line;
            while(getline(infile, line)){
                ssim += getSSIMFromLine(line);
                num += 1;
            }

            ssim = ssim/num;
        }

        if( strcmp( entry->d_name, "time.txt" ) == 0){
            foundTime = true;
            string file = "./Clips/" + clip + "/" + codec + "/" + conf + "/";
            file = file + entry->d_name;
            ifstream infile(file);
            string line;
            getline(infile, line);
            time = stof(line);
        }

        if( strcmp( entry->d_name, "stats.txt" ) == 0){
            foundStats = true;
            string file = "./Clips/" + clip + "/" + codec + "/" + conf + "/";
            file = file + entry->d_name;
            ifstream infile(file);
            string line;
            while(getline(infile, line)){
                char c = line.back();
                if(c == 'I'){
                    count_i += 1;
                }
                if(c == 'P'){
                    count_p += 1;
                }
                if(c == 'B'){
                    count_b += 1;
                }


            }

        }
    }
    if(!foundSSIM  || !foundError || !foundTime || !foundStats || count == 0){
        ret.error = true;
    }else{

        ret.error = false;
        ret.count = count;
        ret.psnr = psnr;
        ret.ssim = ssim;


        path = "./Clips/" + clip + "/original.yuv";
        ret.dimOriginale = getFileSize(path)/1000;

        path = "./Clips/" + clip + "/" + codec + "/" + conf + "/out.mp4";
        ret.dimCompressa = getFileSize(path)/1000;

        ret.rapporto = ret.dimOriginale/ret.dimCompressa;

        ret.count_i = count_i;
        ret.count_p = count_p;
        ret.count_b = count_b;

        ret.time = time;
    }


    closedir(dir);
    return ret;
}

void fullCompression(){
    const char *clips[5] = { "Akiyo", "Coastguard", "Container", "Hall", "News" };
    const char *codecs[3] = { "mpeg2video", "libvpx-vp9", "libx264"};
    int confNum = getConfNumber();

    for (int i = 0; i < 5; i++){ //Per ogni clip
        for(int j = 0; j < 3; j++){ //Per ogni codec
            for(int k = 1; k <= confNum; k++){ //Per ogni configurazione
                encode(clips[i], codecs[j], k);
            }
        }
    }
    return;
}

int getConfNumber(){
    struct dirent *entry;
    DIR *dir = opendir("./configurazioni");

    if (dir == NULL) {
        return 0;
    }

    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if( strcmp( entry->d_name, "." ) && strcmp( entry->d_name, ".." ) ){
            count = count + 1;
        }
    }
    closedir(dir);

    return count;
}

void createDirIfNotExists(string path){
    DIR *dir = opendir(path.c_str());

    if (dir == NULL) {
        string comando = "mkdir " + path;
        system(comando.c_str());
    }
}

void createDirs(string clip, string codec, string conf){
    createDirIfNotExists("./Clips");
    createDirIfNotExists("./Clips/" + clip);
    createDirIfNotExists("./Clips/" + clip + "/" + codec);
    createDirIfNotExists("./Clips/" + clip + "/" + codec + "/" + conf);
}

string getConfNameByIndex(int index){

    if(getConfNumber() == 0){
        return "";
    }
    struct dirent *entry;
    DIR *dir = opendir("./configurazioni");

    int indice = 1;
    while ((entry = readdir(dir)) != NULL) {
        if( strcmp( entry->d_name, "." ) && strcmp( entry->d_name, ".." ) ){
            if(indice == index){
                closedir(dir);
                return entry->d_name;
            }
            indice++;
        }
    }

    closedir(dir);
    cout << "ERRORE: Indice Non valido!" << endl;
    return "";
}

int getConfIndexByName(string name){

    struct dirent *entry;
    DIR *dir = opendir("./configurazioni");

    int indice = 1;
    while ((entry = readdir(dir)) != NULL) {
        if( strcmp( entry->d_name, "." ) && strcmp( entry->d_name, ".." ) ){
            if(strcmp(entry->d_name, name.c_str()) == 0){
                closedir(dir);
                return indice;
            }
            indice++;
        }
    }

    closedir(dir);
    cout << "ERRORE: nome Non valido!" << endl;
    return 0;
}

float getFileSize(string path){
    ifstream myFile(path, ios::binary);
    const auto begin = myFile.tellg();
    myFile.seekg (0, ios::end);
    const auto end = myFile.tellg();
    const auto fsize = (end-begin);

    return fsize;
}

string format(string in, int totalLength){
    int size = in.length();

    if(size%2 != 0){
        in = in + " ";
        size = size + 1;
    }

    if(totalLength%2 != 0){
        totalLength = totalLength + 1;
    }

    totalLength = totalLength -2;

    int spaceNum = (totalLength-size)/2;

    string space = "";

    for(int i = 0; i < spaceNum ; i++){
        space = space + " ";
    }

    return "|" + space + in + space + "|";
}

string formatLeft(string in, int len){
    int size = in.length();

    int spaceNum = ((len - 2)-size);

    string space = "";

    for(int i = 0; i < spaceNum ; i++){
        space = space + " ";
    }

    return "|" + in + space + "|";
}

string printLine(int len){
    if(len %2 != 0){
        len = len + 1;
    }
    string linea = "";
    for(int i = 0; i < len-2 ; i++){
        linea = linea + "-";
    }
    return "+" + linea + "+";
}

string printRound(float var, int decimal){
    float value = (int)(var * 100 + .5);
    string ret = std::to_string((float)value / 100);
    size_t start_pos = ret.find(".");
    int endIndex = start_pos;
    if(decimal > 0 ){
        endIndex = endIndex + decimal + 1;
    }
    return ret.substr(0,endIndex);
}