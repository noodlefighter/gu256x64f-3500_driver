#ifndef GU25664DRV_H_
#define GU25664DRV_H_



#ifdef __cplusplus
extern "C"
{
#endif

int gu25664drv_init (void);
void gu25664drv_clear (void);
void gu25664drv_draw_point (int x, int y, int value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GU25664DRV_H_ */
