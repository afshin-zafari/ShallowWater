#include "util.hpp"
#include "dtsw.hpp"
#include <algorithm>
namespace dtsw{
    /*------------------------------------------*/
    void read_var_D(const char * filename,
                    std::vector< std::pair<uint32_t, uint32_t> > &idx,
                    std::vector< quad<double> > &data)
{
        FILE *f = fopen(filename,"rb");
        uint64_t  N,nnz;
        fread(&N,sizeof(uint64_t),1,f);
        fread(&nnz,sizeof(uint64_t),1,f);

        idx.resize(nnz);
        fread(&idx[0], sizeof(std::pair<uint32_t, uint32_t>), nnz, f) ;

        data.resize(nnz);
        fread(&data[0], sizeof(quad<double>), nnz, f) ;

        fclose(f);

    }
    /*------------------------------------------*/
    void read_var_D_block(const char * filename,
                    std::vector< std::pair<uint32_t, uint32_t> > &idx,
                    std::vector< quad<double> > &data,
                    size_t item_count, size_t block_idx)
{
        FILE *f = fopen(filename,"rb");
        uint64_t  N,nnz;
        fread(&N,sizeof(uint64_t),1,f);
        fread(&nnz,sizeof(uint64_t),1,f);
        long cur = ftell(f);
        size_t index_size = item_count * sizeof(std::pair<uint32_t, uint32_t>);
        idx.resize(item_count);
        fseek(f,block_idx * index_size, SEEK_CUR);
        cur=ftell(f);
        fread(&idx[0], sizeof(std::pair<uint32_t, uint32_t>), item_count, f) ;

        size_t data_size = item_count *sizeof(quad<double>);
        data.resize(item_count);
        fseek(f,sizeof(uint64_t)*2
               + nnz * sizeof(std::pair<uint32_t, uint32_t>)
               + block_idx * data_size ,SEEK_SET);
        cur=ftell(f);
        fread(&data[0], sizeof(quad<double>), item_count, f) ;

        fclose(f);
        (void)cur;
    }
    /*------------------------------------------*/
    void print_index(std::vector< std::pair<uint32_t, uint32_t> > &idx,int from,int N){
        for(int i=from;i<from+N;i++)
            std::cout << "Index ["<<i <<"]= " << idx[i].first <<"," << idx[i].second <<std::endl;
    }
    /*------------------------------------------*/
    void print_data(std::vector< quad<double> > &data,int from,int N){
        for(int i=from;i<from+N;i++){
            std::cout << "Data["<<i <<"] = " ;
            for(int j=0;j<4;j++){
                std::cout << data[i].v[j] << " ";
            }
            std::cout << std::endl;
        }
    }
    /*------------------------------------------*/
    void test_read_sparse(){
        std::vector< std::pair<uint32_t, uint32_t> > index,index2;
        std::vector< dtsw::quad_t > data,data2;
        const char *fn="../galew-6400-31-ep2.7-o4-gc-0.05/D";

        dtsw::read_var_D(fn,index,data);
        int B=100;
        size_t K_MAX = 198400/B;
        for ( size_t K = 0;K < K_MAX; K++){

            dtsw::read_var_D_block(fn,index2,data2,B,K);
            for(int i=0;i < B; i++){
                for(int j=0;j<4;j++){
                    if(std::abs(data2[i].v[j] - data[K*B+i].v[j])>.0){
                        std::cout <<  "K = " << K << " " ;
                        std::cout << "error in Data " << i << "," << j << std::endl;
                        std::cout << std::setw(20) << std::setprecision(10) << (double)(data2[i].v[j] - data[K*B+i].v[j] )<< std::endl;
                    }
                }
                if(index2[i].first != index[K*B+i].first){
                    std::cout << "error in Index " << i << std::endl;
                }

            }
        }
    }
    /*------------------------------------------*/
    void read_var_H_block(const char *filename,
                    std::vector<double> &data,
                    int item_count,
                    int block_index){
        FILE *f = fopen(filename,"rb");
        uint64_t M,N;
        fread(&M,sizeof(uint64_t),1,f);
        fread(&N,sizeof(uint64_t),1,f);
        size_t item_size = sizeof(double) * 4; // each row of H has 4 elements;
        fseek(f,block_index * item_count * item_size  ,SEEK_CUR);
        data.resize(item_count*M);
        fread(&data[0],item_size,item_count,f);
        fclose(f);
    }
    /*------------------------------------------*/
    void read_var_H(const char *filename,
                    std::vector<double> &data){
        FILE *f = fopen(filename,"rb");
        uint64_t M,N;
        fread(&M,sizeof(uint64_t),1,f);
        fread(&N,sizeof(uint64_t),1,f);
        size_t item_size = sizeof(double) ;

        data.resize(M*N);
        fread(&data[0],item_size,M*N,f);
        fclose(f);
    }
    /*------------------------------------------*/
    void test_read_H(){
        std::vector< double > data,data2;
        const char *fn="../galew-6400-31-ep2.7-o4-gc-0.05/H";

        dtsw::read_var_H(fn,data);
        int B=10;
        size_t K_MAX = 6400/B;
        for ( size_t K = 0;K < K_MAX; K++){
            dtsw::read_var_H_block(fn,data2,B,K);
            for(int i=0;i < B; i++){
                for(int j=0;j<4;j++){
                    if(std::abs(data2[i*4+j] - data[K*B*4+i*4+j])>0.0){
                        std::cout <<  "K = " << K << " " ;
                        std::cout << "error in H " << i << "," << j << std::endl;
                        std::cout << std::setw(20) << std::setprecision(10) << (double)(data2[i*4+j] - data[K*B*4+i*4+j])<< std::endl;
                    }
                }
            }
        }
    }
    /*------------------------------------------*/
    void read_var_Atm_block(const char *filename,
                            std::vector<double> &data,
                            int item_count, int block_index){
        FILE *f = fopen(filename,"rb");
        uint64_t M,N;
        fread(&M,sizeof(uint64_t),1,f);
        fread(&N,sizeof(uint64_t),1,f);
        size_t item_size = sizeof(atmdata_t) ;
        fseek(f,block_index * item_count * item_size  ,SEEK_CUR);
        data.resize(item_count*M);
        fread(&data[0],item_size,item_count,f);
        fclose(f);
    }
    /*------------------------------------------*/
    void read_var_Atm(const char *filename,
                        std::vector<double> &data){
        FILE *f = fopen(filename,"rb");
        uint64_t M,N;
        fread(&M,sizeof(uint64_t),1,f);
        fread(&N,sizeof(uint64_t),1,f);
        size_t item_size = sizeof(double) ;

        data.resize(N*M);
        fread(&data[0],item_size,M*N,f);
        fclose(f);
    }
    /*------------------------------------------*/
    void test_read_Atm(){
        std::vector< double > data,data2;
        const char *fn="../galew-6400-31-ep2.7-o4-gc-0.05/atm";

        dtsw::read_var_Atm(fn,data);
        int B=10;
        size_t K_MAX = 6400/B;
        int M = sizeof(atmdata_t)/sizeof(double);
        for ( size_t K = 0;K < K_MAX; K++){
            dtsw::read_var_Atm_block(fn,data2,B,K);
            for(int i=0;i < B; i++){
                for(int j=0;j<M;j++){
                    if(std::abs(data2[i*M+j] - data[K*B*M+i*M+j])>0.0){
                        std::cout <<  "K = " << K << " " ;
                        std::cout << "error in ATM " << i << "," << j << std::endl;
                        std::cout << std::setw(20) << std::setprecision(10) << (double)(data2[i*M+j] - data[K*B*M+i*M+j])<< std::endl;
                    }
                }
            }
        }
    }
    /*------------------------------------------*/
    /*------------------------------------------*/
    int get_owner(int brow, int bcol){
        int r = brow % Parameters.dist.p;
        int c = bcol % Parameters.dist.q;
        int rank = r * Parameters.dist.q+ c;
        return rank == me;
    }
    /*------------------------------------------*/
    struct SparseD{
        std::vector<quad_t> value;
        std::vector<uint32_t> row_index,col_index;
        std::vector<SparseD*> dlist;
        int M,N;
        int brow,bcol;
        SparseD(){}
        SparseD(int rows,int cols){
            M=rows;
            N=cols;
            for(int i=0;i<M*N;i++){
                SparseD *spD = new SparseD();
                spD->brow = i%M;
                spD->bcol = i%N;
                dlist.push_back(spD);
            }
        }
        SparseD &operator()(int i,int j){
            return *dlist[i*M+j];
        }
    };
    /*------------------------------------------*/
    /*------------------------------------------*/
    size_t read_var_D_dist(SparseD &spD){
        std::vector<quad_t> data;
        std::vector<std::pair<uint32_t,uint32_t>> index;
        const char *fn="../galew-6400-31-ep2.7-o4-gc-0.05/D";
        read_var_D(fn,index,data);
        size_t nnz = data.size();
        for(size_t i=0;i<nnz;i++){
            int row = index[i].first;
            int col = index[i].second;
            int brow = row / Parameters.dist.rows_per_block;
            int bcol = col / Parameters.dist.cols_per_block;
            if (!get_owner(brow,bcol))
                continue;

            spD(brow,bcol).value.push_back(data[i]);
            spD(brow,bcol).row_index.push_back(row);
            spD(brow,bcol).col_index.push_back(col);

        }
        return nnz;
    }
    /*------------------------------------------*/
    size_t print_sparse_D(SparseD &spD){
        int M = spD.M;
        int N = spD.N;
        size_t sum =0;
        for(int i=0;i<M;i++){
            for(int j=0;j<N;j++){
                if (spD(i,j).row_index.size() ==0) {
//                    std::cout << std::endl;
                    continue;
                }
                sum +=spD(i,j).row_index.size();
                std::cout << "D(" << i << "," << j << ")" <<
                    " #Rows: " << spD(i,j).row_index.size() <<
                    " #Cols: " << spD(i,j).col_index.size() ;
                auto row_range=std::minmax_element(spD(i,j).row_index.begin(),
                                                   spD(i,j).row_index.end());
                auto col_range=std::minmax_element(spD(i,j).col_index.begin(),
                                                   spD(i,j).col_index.end());
                int rmin = row_range.first  - spD(i,j).row_index.begin();
                int rmax = row_range.second - spD(i,j).row_index.begin();
                int cmin = col_range.first  - spD(i,j).col_index.begin();
                int cmax = col_range.second - spD(i,j).col_index.begin();

                std::cout
                << " Rows = " << spD(i,j).row_index[rmin] << "," << spD(i,j).row_index[rmax]
                << " Cols = " << spD(i,j).col_index[cmin] << "," << spD(i,j).col_index[cmax] << std::endl;
            }
        }
        std::cout << "Total elements: " << sum << std::endl;
        return sum;
    }
    /*------------------------------------------*/
    void test_sparse_d_dist(){
        int M = 6400;
        int N = 6400;;
        Parameters.dist.p = 1;
        Parameters.dist.q = 3;
        Parameters.dist.P = Parameters.dist.p * Parameters.dist.q;
        Parameters.dist.blocks_per_col = 4;
        Parameters.dist.blocks_per_row = 4;
        Parameters.dist.cols_per_block = N / Parameters.dist.blocks_per_col;
        Parameters.dist.rows_per_block = M / Parameters.dist.blocks_per_row;
        size_t sum = 0;
        for (me = 0; me <Parameters.dist.P;me++){
            SparseD *spD = new SparseD(Parameters.dist.blocks_per_row,Parameters.dist.blocks_per_col);
            size_t nnz = read_var_D_dist(*spD);
            std::cout << "NNZ: " << nnz << " rank = " << me <<  std::endl;
            sum += print_sparse_D(*spD);
            delete spD;
        }
        std::cout << "Total elements in all ranks: " << sum << std::endl;
    }
    /*------------------------------------------*/
    void mult_d_h(SparseD &d, double *h){
        for(size_t i=0;i<d.row_index.size();i++){
            int dr=d.row_index[i];
            int dc=d.col_index[i];
            int hc = dc - d.bcol * Parameters.dist.cols_per_block;
            int hr = dr - d.brow * Parameters.dist.rows_per_block;
            h[hr] += d.value[i].v[0] * h[hc];
        }

    }
    /*------------------------------------------*/
    struct HVector{
        std::vector<HVector *> hlist;
        std::vector<double> value;
        int brow,row_start,row_end;
        HVector (){
            row_end = 0;
            row_start = 1<<30;
        }
        HVector(int r){
            for ( int i=0;i<r;i++){
                HVector *h = new HVector();
                h->brow = i;
                hlist.push_back(h);
            }
        }
        HVector &operator ()(int r){
            return * hlist[r];
        }
    };
    /*------------------------------------------*/
    size_t read_var_H_dist(const char *fn, HVector &H){
        std::vector<double> data;
        read_var_H(fn,data);
        int n = data.size();
        for(int i=0;i<n;i++){
            int brow = i /4 / Parameters.dist.rows_per_block;
            if (!get_owner(brow,0))
                continue;
            H(brow).value.push_back(data[i]);
            H(brow).row_start = std::min(H(brow).row_start,i/4);
            H(brow).row_end   = std::max(H(brow).row_end  ,i/4);
        }
        return n;
    }
    /*------------------------------------------*/
    int print_vector_H(HVector &H){
        size_t sum =0 ;//17 may 11Am gyn mottagning kassa
        for(size_t i=0;i<H.hlist.size();i++){
            if ( H.hlist[i]->value.size() ==0)
                continue;
            sum += H.hlist[i]->value.size()/4 ;
            std::cout << "H("<<i<<")" << H.hlist[i]->row_start << "-" << H.hlist[i]->row_end << std::endl;
        }
        std::cout << "Local elements: " << sum << std::endl;
        return sum;
    }
    /*------------------------------------------*/
    void test_vector_H_dist(){
      const char *fn="../galew-6400-31-ep2.7-o4-gc-0.05/H";
        int M = 6400;
        int N = 6400;;
        Parameters.dist.p = 3;
        Parameters.dist.q = 1;
        Parameters.dist.P = Parameters.dist.p * Parameters.dist.q;
        Parameters.dist.blocks_per_col = 4;
        Parameters.dist.blocks_per_row = 4;
        Parameters.dist.cols_per_block = N / Parameters.dist.blocks_per_col;
        Parameters.dist.rows_per_block = M / Parameters.dist.blocks_per_row;
        size_t sum = 0;
        for (me = 0; me <Parameters.dist.P;me++){
            HVector *H= new HVector(Parameters.dist.blocks_per_row);
            size_t n = read_var_H_dist(fn,*H)/4;
            std::cout << "Total elems: " << n << " rank = " << me <<  std::endl;
            sum += print_vector_H(*H);
            delete H;
        }
        std::cout << "Total elements in all ranks: " << sum << std::endl;
    }
    /*------------------------------------------*/
    struct AtmVector{
        std::vector<double> data;
        std::vector<AtmVector *> alist;
        int brow,row_end,row_start;
        AtmVector(){
            row_end = 0;
            row_start = 1<<30;
        }
        AtmVector(int r){
            for(int i=0;i<r;i++){
                AtmVector * v = new AtmVector();
                v->brow = i;
                alist.push_back(v);
            }
        }
        AtmVector &operator()(int r){
            return *alist[r];
        }
    };
    /*------------------------------------------*/
    size_t read_var_atm_dist(const char *fn,AtmVector &A){
        std::vector<double> data;
        read_var_Atm(fn,data);
        int n = data.size();
        for(int i=0;i<n;i++){
            int a_row =i/(sizeof(atmdata_t)/ sizeof(double));
            int brow = a_row / Parameters.dist.rows_per_block;
            if (!get_owner(brow,0)){
                continue;
            }
            A(brow).data.push_back(data[i]);
            A(brow).row_start = std::min(A(brow).row_start,a_row);
            A(brow).row_end   = std::max(A(brow).row_end  ,a_row);
        }
        return n;
    }
    /*------------------------------------------*/
    void test_vector_Atm_dist(){
        int print_vector_Atm(AtmVector &A);
        const char *fn="../galew-6400-31-ep2.7-o4-gc-0.05/atm";
        int M = 6400;
        int N = 6400;
        Parameters.dist.p = 2;
        Parameters.dist.q = 1;
        Parameters.dist.P = Parameters.dist.p * Parameters.dist.q;
        Parameters.dist.blocks_per_col = 4;
        Parameters.dist.blocks_per_row = 4;
        Parameters.dist.cols_per_block = N / Parameters.dist.blocks_per_col;
        Parameters.dist.rows_per_block = M / Parameters.dist.blocks_per_row;
        size_t sum = 0;
        for (me = 0; me <Parameters.dist.P;me++){
            AtmVector *A= new AtmVector(Parameters.dist.blocks_per_row);
            size_t n = read_var_atm_dist(fn,*A)/sizeof(atmdata_t);
            n = print_vector_Atm(*A);
            std::cout << "Total elems: " << n << " in rank " << me <<  std::endl;
            sum += n;
            delete A;
        }
        std::cout << "\nTotal elements in all ranks: " << sum << std::endl;
    }
    /*------------------------------------------*/
    int print_vector_Atm(AtmVector &A){
        size_t sum =0 ;//17 may 11Am gyn mottagning kassa
        for(size_t i=0;i<A.alist.size();i++){
            if ( A.alist[i]->data.size() ==0)
                continue;
            sum += A.alist[i]->data.size()/sizeof(atmdata_t)*sizeof(double);
            std::cout << "A("<<i<<")" << A.alist[i]->row_start << "-" << A.alist[i]->row_end << std::endl;
        }
        std::cout << "Local elements: " << sum << std::endl;
        return sum;
    }
    /*------------------------------------------*/
    /*------------------------------------------*/
}
