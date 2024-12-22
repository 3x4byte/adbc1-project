import { createRouter, createWebHistory } from 'vue-router'
import LoginView from '@/views/LoginView.vue';
import DashboardView from '@/views/DashboardView.vue';
import LogoutView from '@/views/LogoutView.vue';

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/login',
      name: 'Login',
      component: LoginView,
    },
    {
      path: '/logout',
      name: 'Logout',
      component: LogoutView,
    },
    {
      path: '/dashboard',
      component: DashboardView,
      meta: { requiresAuth: true }, // only signed in users can access
    },
    {
      path: '/:catchAll(.*)', // unknown routes
      redirect: '/dashboard',     // go to /dashboard
    },
  ],
});

// Globale Navigation Guard
router.beforeEach((to, _, next) => {
  const isAuthenticated = localStorage.getItem('pocketbase_auth'); // auth sample
  if (to.meta.requiresAuth && !isAuthenticated) {
    next('/login');
  } else {
    next();
  }
});

export default router
