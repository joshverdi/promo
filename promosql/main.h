#ifndef MAIN_H
#define MAIN_H

#include <boost/assign/list_of.hpp>
#include <vector>
#include <QVector>
#include "qgl.h"
#include "CGLA/Vec3f.h"
#include "CGLA/Mat3x3f.h"

#ifdef PROCEDURAL_MAX
#undef PROCEDURAL_MAX
#endif

#ifdef PROCEDURAL_MIN
#undef PROCEDURAL_MIN
#endif

#define PROCEDURAL_MAX(a, b) ((a)>(b)?(a):(b))
#define PROCEDURAL_MIN(a, b) ((a)<(b)?(a):(b))

#define DB_TYPE         "QSQLITE"
#define DFLT_USERNAME   "promouser"
#define DFLT_PASSWORD   "promopassword"
#define DFLT_DBNAME     "promo_groupdb"
#define DFLT_HOSTNAME   "localhost"
#define IMGGRP_TBLNAME  "ImageGroup"
#define QLFCTN_TBLNAME  "Qualification"
#define DFLT_CSVOUTPUT  "promo_output.csv"

#define LOCK_VIEW_ON_X 0
#define LOCK_VIEW_ON_Y 1
#define LOCK_VIEW_ON_Z 2
#define CONSTRAINTS_FREE_VIEW 3
#define MATRIX_DIM 3

static const int IMAGE_NB = 27;
const uint MX_ORDERING[6][IMAGE_NB] =
                       {        {1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
                                {1,  4,  7,  2,  5,  8,  3,  6,  9, 10, 13, 16, 11, 14, 17, 12, 15, 18, 19, 22, 25, 20, 23, 26, 21, 24, 27},
                                {1,  4,  7, 10, 13, 16, 19, 22, 25,  2,  5,  8, 11, 14, 17, 20, 23, 26,  3,  6,  9, 12, 15, 18, 21, 24, 27},
                                {1,  2,  3, 10, 11, 12, 19, 20, 21,  4,  5,  6, 13, 14, 15, 22, 23, 24,  7,  8,  9, 16, 17, 18, 25, 26, 27},
                                {1, 10, 19,  2, 11, 20,  3, 12, 21,  4, 13, 22,  5, 14, 23,  6, 15, 24,  7, 16, 25,  8, 17, 26,  9, 18, 27},
                                {1, 10, 19,  4, 13, 22,  7, 16, 25,  2, 11, 20,  5, 14, 23,  8, 17, 26,  3, 12, 21,  6, 15, 24,  9, 18, 27}
                       };

class PImgSet;
class PQuestionnaire;
class PDbSetup;

enum {  HELP_CMD    = 'h',
        DB_CMD      = 'd',
        MX_CMD      = 'x',
        SHTBL_CMD   = 's',
        CSVG_CMD    = 'g',
        USR_CMD     = 'u',
        PWD_CMD     = 'p',
        CSVT_CMD    = 'o',
        ITBL_CMD    = 't',
        GRAM_CMD    = 'a',
        UID_CMD     = 'i',
        QLF_CMD     = 'y',
        V3D_CMD     = 'v',
        QTBL_CMD    = 'l',
        IMGNB_CMD   = 'n',
        USEF_CMD    = 'z',
        QLFR_CMD    = 'w',
        DIFF_CMD    = 'f',
        QCOO_CMD    = 'c',
        BBOX_CMD    = 'b'
             };

static const std::vector<std::string> vec_commands =
        boost::assign::list_of  ("help")
                                ("databasename")
                                ("matrixoutput")
                                ("showtable")
                                ("csvgroupname")
                                ("username")
                                ("password")
                                ("csvouttable")
                                ("imgtablename")
                                ("grammar")
                                ("userid")
                                ("qualify")
                                ("view3d")
                                ("qlftablename")
                                ("imgnb")
                                ("usefulness")
                                ("qualifier")
                                ("difference")
                                ("qlfrcoords")
                                ("boundingbox")
;
static const std::vector<char> vec_shortcommands =
        boost::assign::list_of  ((char) HELP_CMD)
                                ((char) DB_CMD)
                                ((char) MX_CMD)
                                ((char) SHTBL_CMD)
                                ((char) CSVG_CMD)
                                ((char) USR_CMD)
                                ((char) PWD_CMD)
                                ((char) CSVT_CMD)
                                ((char) ITBL_CMD)
                                ((char) GRAM_CMD)
                                ((char) UID_CMD)
                                ((char) QLF_CMD)
                                ((char) V3D_CMD)
                                ((char) QTBL_CMD)
                                ((char) IMGNB_CMD)
                                ((char) USEF_CMD)
                                ((char) QLFR_CMD)
                                ((char) DIFF_CMD)
                                ((char) QCOO_CMD)
                                ((char) BBOX_CMD)
;



struct command
{
    std::string dbname;
    std::string csvinput;
    std::string matrixfilename;
    std::string imgtablename;
    std::string username;
    std::string password;
    std::string csvoutput;
    std::string grammar;
    std::string qlftablename;
    std::string usefulfn;
    std::string qualifierfn;
    std::string differencefn;
    std::string bboxfn;
    int userid;
    int imgid;
};

class PBoundingBox;

void showhelpstring();
void showtable(std::string _tablename);
void csv_table(std::string _csvoutputfn, std::string _tablename);

void eigen_decomposition(double A[MATRIX_DIM][MATRIX_DIM], double V[MATRIX_DIM][MATRIX_DIM], double d[MATRIX_DIM]);
void tql2(double V[MATRIX_DIM][MATRIX_DIM], double d[MATRIX_DIM], double e[MATRIX_DIM]);
void tred2(double V[MATRIX_DIM][MATRIX_DIM], double d[MATRIX_DIM], double e[MATRIX_DIM]);
double hypot2(double x, double y);
CGLA::Vec3f translate(CGLA::Vec3f _point, CGLA::Vec3f _vec);

std::string                 parse_command(int argc, char *argv[]);  // Returns the options list
std::vector<std::string>    get_grammars(int _user_id, QString _tablename=IMGGRP_TBLNAME);
std::vector<int>            get_users(QString _grammar, QString _tablename=IMGGRP_TBLNAME);
QVector< QVector<int> >     generate_matrix(int _user_id, QString _grammar, QString _tablename=IMGGRP_TBLNAME);
QVector< QVector<int> >     generate_matrix(QString _mxordering_inputfn, int _user_id, QString _grammar, QString _tablename=IMGGRP_TBLNAME);
QVector< QVector< QVector<int> > > generate_switchedmatrix(int _user_id, QString _grammar, QString _tablename=IMGGRP_TBLNAME);
QVector< QVector< QVector<int> > > generate_resampldmatrix(std::vector< std::vector<uint> > _vecs_indices, int _user_id, QString _grammar, QString _tablename=IMGGRP_TBLNAME);
//QVector< QVector<float> >   generate_diffmatrix(int _user_id, QString _grammar, QString _imggrptable=IMGGRP_TBLNAME, QString _qlfrtable=QLFCTN_TBLNAME);
std::vector<PImgSet>        request_imgset(int _user_id, QString _grammar, QString _tablename=IMGGRP_TBLNAME);
std::vector<PQuestionnaire> request_questnr(int _user_id, QString _grammar, QString _tablename=QLFCTN_TBLNAME);
void qualify(int _user_id, QString _grammar, std::vector<uint> _vec_groups, const QString _tablename=QLFCTN_TBLNAME);
int get_qlfr();
std::string get_customqlfr();

std::vector<float> compute_usefulness(int _user_id, QString _grammar, QString _imggrptable=IMGGRP_TBLNAME, QString _qlfrtable=QLFCTN_TBLNAME);
std::vector<float> compute_difference(int _user_id, QString _grammar, QString _imggrptable=IMGGRP_TBLNAME, QString _qlfrtable=QLFCTN_TBLNAME);
QVector<QStringList*> check_qualifiers(int _user_id, QString _grammar, QString _imggrptable=IMGGRP_TBLNAME, QString _qlfrtable=QLFCTN_TBLNAME);
void load_wrappers(std::vector<float>& _vec_extremum, std::vector< std::vector<float> >& _vec_nearestpt);
void load_scattered(std::vector< std::vector<float> >& _vec_scatterplot);
void multimx_to_csv(const QVector< QVector< QVector<int> > >& _matrices, QString _filebasename, const uint _mx_ordering[6][IMAGE_NB]);
void multimx_to_csv(const QVector< QVector< QVector<int> > >& _matrices, QString _filebasename, std::vector< std::vector<uint> > _vec_headeridx);
void qlfrs_to_csv(QString _tablename=QLFCTN_TBLNAME);
void qlfrs_to_csv_annotated(QString _tablename=QLFCTN_TBLNAME);
void save_qlfr_coords(std::vector<uint> _vec_resampld_idx, QVector<QStringList*> _qvec_qlfrs);
std::vector<PBoundingBox> qlfr_obb(const QStringList& _qlfr_list, std::vector< std::vector< std::vector<GLfloat> > >& _vecs_obbclouds);
QStringList load_obbqlfr(QString _obbfilter_filename);
std::vector< std::vector<float> > hsv_colorgenerator(unsigned int _number);
std::vector< std::vector<float> > rgb_colorgenerator(unsigned int _number);
void hsv_to_rgb( float *r, float *g, float *b, float h, float s, float v );
QString rgb_to_qstr(float _r, float _g, float _b);
bool synonyms(std::string _word1, std::string _word2);
#endif // MAIN_H
