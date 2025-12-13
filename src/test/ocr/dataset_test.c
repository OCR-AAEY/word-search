#include <criterion/criterion.h>
#include <math.h>
#include <stdio.h>

#include "matrix/matrix.h"
#include "ocr/dataset.h"
#include "test_settings.h"

Test(dataset, ds_save_and_load_random_test)
{
    REPEAT
    {
        Dataset *ds = ds_create_empty();

        for (size_t i = 0; i < 100; i++)
        {
            Matrix *m = mat_create_random_uniform(28, 28, 0.0f, 1.0f);
            mat_inplace_to_one_hot(m);

            Training_Data *td = td_create(m, i % 26);
            ds_add_tuple(ds, td);
        }

        const char *tmpfile = "save_and_load_random_test.dataset";
        ds_save_to_compressed_file(ds, tmpfile);

        Dataset *loaded = ds_load_from_compressed_file((char *)tmpfile);

        cr_assert_eq(ds_size(ds), ds_size(loaded),
                     "Dataset sizes differ: %zu vs %zu", ds_size(ds),
                     ds_size(loaded));

        for (size_t i = 0; i < ds_size(ds); i++)
        {
            Training_Data *orig = ds_get_data(ds, i);
            Training_Data *copy = ds_get_data(loaded, i);

            cr_assert_eq(orig->expected_class, copy->expected_class,
                         "Class mismatch at index %zu: %zu vs %zu", i,
                         orig->expected_class, copy->expected_class);

            Matrix *mo = orig->input;
            Matrix *mc = copy->input;
            float *co = mat_coef_ptr(mo, 0, 0);
            float *cc = mat_coef_ptr(mc, 0, 0);

            for (int p = 0; p < 28 * 28; p++)
            {
                cr_assert_float_eq(
                    co[p], cc[p], 0.0001,
                    "Pixel mismatch at sample %zu, pixel %d: %f vs %f", i, p,
                    co[p], cc[p]);
            }
        }

        ds_free(ds);
        ds_free(loaded);
    }
}
