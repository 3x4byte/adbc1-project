<template>
  <div class="login-container">
    <h1>Login</h1>
    <form @submit.prevent="handleLogin">
      <div class="input-group">
        <label for="username">Benutzername</label>
        <input type="text" id="username" v-model="username" required />
      </div>
      <div class="input-group">
        <label for="password">Passwort</label>
        <input type="password" id="password" v-model="password" required />
      </div>
      <button type="submit" :disabled="isLoading">{{ isLoading ? 'Anmelden...' : 'Anmelden' }}</button>
      <p v-if="errorMessage" class="error">{{ errorMessage }}</p>
    </form>
  </div>
</template>
  
<script>
  import { defineComponent } from 'vue';
  
  export default defineComponent({
    name: 'LoginView',
    data() {
      return {
        username: '',
        password: '',
        isLoading: false,
        errorMessage: '',
      };
    },
    methods: {
      async handleLogin() {
        this.isLoading = true;
        this.errorMessage = '';
        try {
          await this.$pb.collection("users").authWithPassword(this.username, this.password);

          if (this.$pb.authStore.isValid) {
            this.$router.push('/dashboard'); // Weiterleiten
          } else {
            this.errorMessage('Ungültige Anmeldedaten');
          }
        }catch(error) {
          this.errorMessage = 'Fehler bei der Anmeldung. Bitte versuchen Sie es erneut.';
        } finally {
          this.isLoading = false;
        }
      },
    },
  });
</script>
  
<style scoped>
  .login-container {
    max-width: 400px;
    margin: 50px auto;
    padding: 30px;
    border: 1pt solid var(--color-border);
    border-radius: 12px;
    text-align: center;
  }
  .input-group {
    display: flex;
    flex-direction: column;
    align-items: flex-start;
    margin-bottom: 15px;
  }
  .input-group label {
    margin-bottom: 5px;
    font-weight: bold;
  }
  .input-group input {
    width: 100%;
    padding: 8px;
    border: 1px solid var(--color-border);
    border-radius: 5px;
  }
  .error {
    color: red;
    margin-top: 10px;
  }
</style>